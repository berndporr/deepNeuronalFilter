# Audio noise remover

## Input files

Input files are in the `audio` folder. The `signal_noise.wav`
file is the processed by `audio_filter`. Channel layout of the
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

## Results
The results are created in the `results` folder:

`dnf_out.dat  dnf_remover.dat  lms_out.dat  lms_remover.dat  noiseref.dat  signalWithNoise.dat  weight_distance.tsv`.

The `.dat` files are audio files which can be played back with `sox`. For example:

```
sox dnf_out.dat -d
```

playback can be started even during filtering.
