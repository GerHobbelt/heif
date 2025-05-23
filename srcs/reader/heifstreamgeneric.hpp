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

#ifndef HEIFSTREAMGENERIC_HPP_
#define HEIFSTREAMGENERIC_HPP_

namespace HEIF
{
    class StreamInterface;

    StreamInterface* openFile(const char* filename);
}  // namespace HEIF

#endif  // HEIFSTREAMGENERIC_HPP_
