import os
os.environ["CUDA_VISIBLE_DEVICES"] = "-1"
import ROOT
import numpy as np
import matplotlib.pyplot as plt
import tensorflow as tf
from tensorflow import keras
from keras import models
from keras import layers
from joblib import load, dump
from sklearn.preprocessing import QuantileTransformer

#load data we used to train the neural network
training_data = ROOT.RDataFrame("rec_tree", "../pip-data.root")
vars = [element for element in training_data.GetColumnNames()]


#Load the neural networks trained on the three different types of data
detected_DNN = keras.models.load_model('detected/kerasDNN')
calculated_DNN = keras.models.load_model('calculated/kerasDNN')
truth_DNN = keras.models.load_model('truth/kerasDNN')

#generate some random data distribution
p = np.full((1000,),10)
theta = np.full(p.shape,0.4)
phi = np.full(p.shape,1.1)

test_data = np.vstack((p,theta,phi)).T
##print(test_data)

#calculate the probabilities from each of the models
probs_detected = detected_DNN.predict(test_data)
probs_calcualted = calculated_DNN.predict(test_data)
probs_truth = truth_DNN.predict(test_data)

detected_weights = probs_detected/(1-probs_detected)
calculated_weights = probs_calcualted/(1-probs_calcualted)
truth_weights = probs_truth/(1-probs_truth)

##print(p.shape[0])

n, bins, patches = plt.hist(p,p.shape[0], density=True, weights=detected_weights)

plt.savefig('plots/detected.png')