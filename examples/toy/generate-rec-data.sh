#!/bin/bash

#This file will run the scripts that will produce the reconstructed data for each particle where all the 
#other final state particles are detected and used to calculate it's final particle

mkdir mass_plots

root -q proton-momentum.C
root -q pip-momentum.C
root -q pim-momentum.C