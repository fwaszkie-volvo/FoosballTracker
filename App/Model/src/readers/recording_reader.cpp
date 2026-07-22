#include "recording_reader.hpp"

bool RecordingReader::Open(const std::string& source) { return capture_.open(source); }
