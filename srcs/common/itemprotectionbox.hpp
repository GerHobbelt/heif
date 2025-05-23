/* This file is part of Nokia HEIF library
 *
 * Copyright (c) 2015-2025 Nokia Corporation and/or its subsidiary(-ies). All rights reserved.
 *
 * Contact: heif@nokia.com
 *
 * This software, including documentation, is protected by copyright controlled by Nokia Corporation and/ or its
 * subsidiaries. All rights are reserved.
 *
 * Copying, including reproducing, storing, adapting or translating, any or all of this material requires the prior
 * written consent of Nokia.
 */

#ifndef ITEMPROTECTIONBOX_HPP
#define ITEMPROTECTIONBOX_HPP

#include "customallocator.hpp"
#include "fullbox.hpp"
#include "protectionschemeinfobox.hpp"

/** Item Protection Box class
 * @details 'ipro' box implementation as specified in the ISOBMFF specification. */
class ItemProtectionBox : public FullBox
{
public:
    ItemProtectionBox();
    ~ItemProtectionBox() override = default;

    /** @return Number of contained Protection Scheme Info Boxes */
    std::uint16_t getProtectionCount() const;

    /** Get a ProtectionSchemeInfoBox box.
     *  @param [in] index 0-based index of the box */
    const ProtectionSchemeInfoBox& getEntry(std::uint16_t index) const;

    /** Add a new ProtectionSchemeInfoBox box.
     *  @param [in] sinf The ProtectionSchemeInfoBox to add
     *  @return 0-based index of the added box */
    std::uint16_t addEntry(const ProtectionSchemeInfoBox& sinf);

    /** Write box to ISOBMFF::BitStream.
     *  @see Box::writeBox() */
    void writeBox(ISOBMFF::BitStream& bitstream) const override;

    /** Read box from ISOBMFF::BitStream.
     *  @see Box::parseBox() */
    void parseBox(ISOBMFF::BitStream& bitstream) override;

private:
    Vector<ProtectionSchemeInfoBox> mProtectionInformation;  ///< 'sinf' boxes
};

#endif /* ITEMPROTECTIONBOX_HPP */
