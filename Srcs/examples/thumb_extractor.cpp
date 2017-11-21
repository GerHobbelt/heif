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

using namespace std;

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

vector<unsigned char> getExif(HevcImageFileReader &reader, uint32_t contextId) {
    ImageFileReaderInterface::IdVector gridItemIds;
    reader.getItemListByType(contextId, "grid", gridItemIds);
    ImageFileReaderInterface::IdVector cdscItemIds;
    reader.getReferencedToItemListByType(contextId, gridItemIds.at(0), "cdsc", cdscItemIds);
    ImageFileReaderInterface::DataVector data;
    reader.getItemData(contextId, cdscItemIds.at(0), data);    
    return data;
} 

void writeExif(vector<unsigned char> exif) {
    //exif
    ofstream ofileExif("metadata.exif");
    ofileExif.write((char*) &exif[0], exif.size());
    ofileExif.close();
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

    auto exif = getExif(reader, contextId);

    writeExif(exif);

    cout << "All done!" << endl;

    reader.close();
    return 0;
}



