# c-offshore-vibration-spectrum

Extract RMS, crest factor and dominant vibration frequency from uniformly sampled accelerometer captures.

```sh
make test
./app < examples/input.txt
```

Offline diagnostic tool. Inputs and limits must be validated for each installation; no equipment commands are sent.

MIT. Built by [brunnodev](https://brunnodev.store).

## Execution proof

[![Executable proof](https://github.com/brunnojob/c-offshore-vibration-spectrum/actions/workflows/proof.yml/badge.svg)](https://github.com/brunnojob/c-offshore-vibration-spectrum/actions/workflows/proof.yml)

![Recorded program execution](docs/proof/execution.svg)

[Verified run](https://github.com/brunnojob/c-offshore-vibration-spectrum/actions/runs/38018107474) · [Execution report](docs/proof/evidence.json)

Run `python .proof/record.py` after installing the prerequisites above. The scenarios execute repository code and verify exit codes and expected output. CI publishes `execution-proof` with the transcript, input fingerprints and source commit. The downloadable report identifies the exact tested version; the workflow badge tracks the latest run.
