# Sample Math Plugin [![DOI](https://zenodo.org/badge/130402352.svg)](https://zenodo.org/badge/latestdoi/130402352)

Plugin for the Open Ephys GUI to perform simple samplewise arithmetic (addition, subtraction, multiplication, division, summation and averaging) on continuous channels. For binary operations, the second argument can be either a constant or another channel.

![Sample Math with constant](sm_const.png) ![Sample Math with channel](sm_chan.png)

## Installing with CMake:

This plugin can now be built outside of the main GUI file tree using CMake. In order to do so, it must be in a sibling directory to plugin-GUI\* and the main GUI must have already been compiled.

You should use this branch if you are already using CMake to build the *main GUI* (in development as of writing). Otherwise, use the `master` branch.

See `SampleMath/CMAKE_README.txt` and/or the wiki page [here](https://open-ephys.atlassian.net/wiki/spaces/OEW/pages/1259110401/Plugin+CMake+Builds) for build instructions.

\* If you have the GUI built somewhere else, you can specify its location by setting the environment variable `GUI_BASE_DIR` or defining it when calling cmake with the option `-DGUI_BASE_DIR=<location>`.

## Usage:

* Top row: select which operation to perform on each processed input channel. The first four operations transform selected channels according to the second operand as selected below, whereas "SUM" and "MEAN" output the sum or mean of all selected channels on each selected channel.
* Middle row: select whether the second operand is constant or another channel.
* Bottom row: select that constant or channel.
* Channels can be included/excluded from processing by selecting/deselecting them in the "Param" section of the drawer. NOTE however that if a channel is selected as the second argument of the operation, only channels from the same source node and subprocessor may be included for processing. This is to ensure that the channels input to an operation always have the same number of samples per buffer (the plugin does not do any resampling).

## Testing (requires MATLAB and a connected acquisition board):

1. Open Open Ephys version 0.4.3.3 (master branch at time of writing)
2. Load the configuration file `Test/settings.xml`
3. Record for however long you want to test - probably less than a minute. The Rhythm FPGA and each instance of Sample Math will make its own recording of channels 1 and 2.
4. In MATLAB, run `check_sample_math_output(<recording directory>)`. It should plot a figure similar to `example_check_results.fig`, showing the error on each channel for each operation. Ensure that the errors are reasonably small.
