#ifndef GLOBALS_H
#define GLOBALS_H

// Session parameters
#define SESSION_FILE_DIR QDir::current().absolutePath() + "/sessions/"

#define SESSION_FILE_EXT ".ss"

#define DATA_FILE_EXT ".raw"

// Plotting parameters
static constexpr unsigned kPlotDelay = 500;

#endif // GLOBALS_H
