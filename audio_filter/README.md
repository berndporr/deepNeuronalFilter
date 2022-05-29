# Audio noise remover

## Input files

Input files are in the `audio` folder. The `signal_noise.wav`
file is the WAV processed by `audio_filter`. Channel layout of the
WAV files:

- left: signal + noise
 - right: noise



## Running the program
```
Usage: ./audio_filter [-a] [-b] [<expNumber>]
       -a calculates all experiments one by one without screen output.
       -b calculates all experiments multi-threaded without screen output.
       Press ESC in the plot window to cancel the program.
```

For example `./audio_filter 1` processes the audio of the 1st
experiment in a GUI. `./audio_filter -b` processes all experiments
multi-threaded without a GUI.

## Results
The results are created in the `results` folder:

 - `dnf_out.dat` : Output of the DNF
 - `lms_out.dat` : Output of the LMS/FIR filter
 - `noiseref.dat`: Noise reference (input)
 - `signalWithNoise.dat` : Signal + noise (input)
 - `weight_distance.tsv` : Weight distance. Format: d_all,d0,d1,d2,d3,...

The `.dat` files are audio files which can be played back with `sox`. For example:

```
sox dnf_out.dat -d
```

playback can be started even during filtering, especially when using the `-b` option.

