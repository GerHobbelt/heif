/* Copyright (c) 2015, Nokia Technologies Ltd.
 * All rights reserved.
 *
 * Licensed under the Nokia High-Efficiency Image File Format (HEIF) License (the "License").
 *
 * You may not use the High-Efficiency Image File Format except in compliance with the License.
 * The License accompanies the software and can be found in the file "LICENSE.TXT".
 *
 * You may also obtain the License at:
 * https://nokiatech.github.io/heif/license.txt
 */

/** This file includes some examples about using the reader API
 *  Note that binary execution fails if proper .heic files are not located in the directory */

#include "hevcimagefilereader.hpp"
#include <iostream>
#include <boost/interprocess/streams/bufferstream.hpp>
#include <fstream>
#include "json.hpp"

using namespace std;
using json = nlohmann::json;


struct Metadata {
    uint8_t rows;
    uint8_t cols;    
    vector<uint32_t> tileIndexes;
    vector<vector<unsigned char>> tileBlobs;
    vector<unsigned char> exif;
};

vector<char> readFromStdin() {
    char input[1024];
    size_t readBufSize;
    vector<char> buffer;
    bool done = false;
    while (!done) {
        cin.read(input, sizeof(input));
        readBufSize = cin.gcount();
        if(cin.fail() || cin.bad() || cin.eof()) {
            if (std::cin.eof()) {
                done = true;
            }
            cin.clear();
        }
        for (size_t i = 0; i < readBufSize; i++) {
            buffer.push_back(input[i]);
        }
    }
    return buffer;
}

Metadata fetchMetadata(HevcImageFileReader &reader, uint32_t contextId) {
    ImageFileReaderInterface::GridItem gridItem;
    ImageFileReaderInterface::IdVector gridItemIds;
    Metadata metadata = {};

    //get all grid items
    reader.getItemListByType(contextId, "grid", gridItemIds);
    gridItem = reader.getItemGrid(contextId, gridItemIds.at(0));
    metadata.rows = gridItem.rowsMinusOne;
    metadata.cols = gridItem.columnsMinusOne;

    return metadata;
}

vector<unsigned char> getExif(HevcImageFileReader &reader, uint32_t contextId) {
    ImageFileReaderInterface::IdVector gridItemIds;
    reader.getItemListByType(contextId, "grid", gridItemIds);
    ImageFileReaderInterface::IdVector cdscItemIds;
    reader.getReferencedToItemListByType(contextId, gridItemIds.at(0), "cdsc", cdscItemIds);
    ImageFileReaderInterface::DataVector data;
    reader.getItemData(contextId, cdscItemIds.at(0), data);    
    return data;
} 

vector<uint32_t> getTileIndexes(HevcImageFileReader &reader, uint32_t contextId) {
    ImageFileReaderInterface::IdVector masterItemIds;
    vector<uint32_t> tileIndexes;
    reader.getItemListByType(contextId, "master", masterItemIds);
    for (const auto masterId : masterItemIds) {
        tileIndexes.push_back(masterId);
    } 
    return tileIndexes;
}

vector<vector<unsigned char>> getTileBlobs(HevcImageFileReader &reader, uint32_t contextId) {
    ImageFileReaderInterface::DataVector data;
    ImageFileReaderInterface::IdVector masterItemIds;    
    vector<vector<unsigned char>> tileBlobs;
    reader.getItemListByType(contextId, "master", masterItemIds);
    for (const auto masterId : masterItemIds) {
        reader.getItemDataWithDecoderParameters(contextId, masterId, data);
        tileBlobs.push_back(data);
    } 
    return tileBlobs;
}

void writeMetadataToDisk(Metadata metadata) {
    json j;
    j["number_of_tiles"] = metadata.tileIndexes.size();
    j["rows"] = metadata.rows+1;
    j["cols"] = metadata.cols+1;
    ofstream ofileMeta("metadata_tiles.json");
    ofileMeta << j;
    ofileMeta.close();

    //exif
    ofstream ofileExif("metadata.exif");
    ofileExif.write((char*) &metadata.exif[0], metadata.exif.size());
    ofileExif.close();    
}

void writeTilesToDisk(Metadata metadata) {
    for (size_t i = 0; i < metadata.tileIndexes.size(); i++) {
        char buff[100];
        snprintf(buff, sizeof(buff), "%d", metadata.tileIndexes[i]);
        string tilename = buff;        
        ofstream ofile(tilename);
        ofile.write((char*) &metadata.tileBlobs[i][0], metadata.tileBlobs[i].size());
        ofile.close();
    }
}

int main() {
    auto buffer = readFromStdin();

    HevcImageFileReader reader;
    boost::interprocess::bufferstream input_stream(&buffer[0], buffer.size());
    reader.initialize(input_stream);
    
    // Verify that the file has one or several images in the MetaBox
    const auto& properties = reader.getFileProperties();
    if (!properties.fileFeature.hasFeature(ImageFileReaderInterface::FileFeature::HasSingleImage) && 
        !properties.fileFeature.hasFeature(ImageFileReaderInterface::FileFeature::HasImageCollection) && 
        !properties.fileFeature.hasFeature(ImageFileReaderInterface::FileFeature::HasImageSequence)) {
        return 1;
    }

    // Find the item ID of the first master image
    const uint32_t contextId = properties.rootLevelMetaBoxProperties.contextId;

    auto metadata = fetchMetadata(reader, contextId);

    metadata.tileIndexes = getTileIndexes(reader, contextId);

    metadata.tileBlobs = getTileBlobs(reader, contextId);

    metadata.exif = getExif(reader, contextId);

    if (metadata.tileIndexes.size() != metadata.tileBlobs.size()) {
        return 1;
    }

    writeMetadataToDisk(metadata);

    writeTilesToDisk(metadata);

    cout << "All done!" << endl;

    reader.close();
    return 0;
}



