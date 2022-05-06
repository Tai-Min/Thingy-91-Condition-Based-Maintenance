# Thingy-91-Condition-Based-Maintenance

Repository for [Condition based maincenance](TODO), which was submitted to [Make it smart with Nordic Thingy:91](https://www.electromaker.io/contest/make-it-smart-with-nordic-thingy-91).

Simple Thingy:91 based IoT sensor for condition based maintenance of power generators. The sensor uses built in high G accelerometer to perform vibration classification using FFT, PCA and simple feedforward network on the edge. Sensor's outputs are LED visuals along with MQTT messages sent over LTE.

Sensor's pipeline can be seen below:
![Sensor's pipeline](pipeline.jpg)

This repo contains files for the Thingy:91 application itself (thingy/application), PCA model generator (thingy/pca_dataset_collector), neural dataset collector (thingy/neural_dataset_collector) and design files for Arduino based testbench to test the sensor in controlled environment (workbench/).
