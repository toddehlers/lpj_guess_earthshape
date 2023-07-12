//  This Source Code Form is subject to the terms of the Mozilla Public
//  License, v. 2.0. If a copy of the MPL was not distributed with this
//  file, You can obtain one at http://mozilla.org/MPL/2.0/


////////////////////////////////////////////////////////////////////////////////
/// \file partitionedmapserializer.cpp
/// \brief Implementation file for PartitionedMapSerializer/Deserializer
///
/// Since these classes are templates, most of their implementation is in the
/// header.
///
/// \author Joe Siltberg
/// $Date$
///
////////////////////////////////////////////////////////////////////////////////

#include "partitionedmapserializer.h"
#include <sstream>

std::string create_path(const char* directory, int rank) {
	std::ostringstream os;
	os << directory << "/" << rank << ".state";
	return os.str();
}
