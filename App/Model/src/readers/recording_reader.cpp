#include "recording_reader.hpp"

#include <opencv2/videoio.hpp>

bool RecordingReader::Open(const std::string& source) { return capture_.open(source); }
