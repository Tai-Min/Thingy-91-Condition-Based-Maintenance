# Thingy-91-Condition-Based-Maintenance

Repository for [Condition based maincenance](https://www.electromaker.io/project/view/contactless-condition-based-maintenance-of-dc-motor), which was submitted for [Make it smart with Nordic Thingy:91](https://www.electromaker.io/contest/make-it-smart-with-nordic-thingy-91) and has won "Edge Impulse Machine Learning Prize".

Simple Thingy:91 based IoT sensor for condition based maintenance of power generators. The sensor uses built in high G accelerometer to perform vibration classification using FFT, PCA and simple feedforward network on the edge. Sensor's outputs are LED visuals along with MQTT messages sent over LTE.

This repository contains files for the uC application and simple Arduino based test bench.

![Testbench](testbench.jpg)

## Sensor's pipeline
Sensor's pipeline can be seen below:
![Sensor's pipeline](pipeline.jpg)

## Folder structure
This repo contains project files for the Thingy:91 application itself (thingy/application folder), PCA model generator (thingy/pca_dataset_collector folder), neural dataset collector (thingy/neural_dataset_collector folder) and design files for Arduino based testbench to test the sensor in controlled environment (workbench folder).

### Thingy:91 sensor project files

### Test bench project files

