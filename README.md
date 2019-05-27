# Conveyor belt with material-based item sorting

## Introduction 
This project implements the AVR-based control system for a conveyor belt with item-sorting capabilities.
Items placed at one of the conveyor belt traverses an array of sensors, is classified by the control system, and is sorted into a bin on a tray actuated by a rotary stepper motor.

Items are sorted by their material type, which is one of Steel, Aluminium, Black Plastic, and White Plastic. Valid items are small cylinders.

The system achieved a maximum speed benchmark of 48 sorted items in 25 seconds (with 1 sort error). At lower speeds, perfect sorting was achieved.

## Platform

This project is meant to run on an AT90USBKey development kit board by Atmel.
It can be built and flashed to the board using the Atmel Studio IDE.
