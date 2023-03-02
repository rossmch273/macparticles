import os
os.environ["CUDA_VISIBLE_DEVICES"] = "-1"
import ROOT
from ROOT import RDataFrame
import numpy as np
import matplotlib.pyplot as plt
import matplotlib as mpl
import tensorflow as tf
from tensorflow import keras
from keras import models
from keras import layers

df_training_data = RDataFrame("tree","correction-training.root")

df_size = df_training_data.Count().GetValue()

count_1e4 = df_training_data.Filter("truDetRatioReweight < 1E4").Count().GetValue()
count_1e3 = df_training_data.Filter("truDetRatioReweight < 1E3").Count().GetValue()
count_1e2 = df_training_data.Filter("truDetRatioReweight < 1E2").Count().GetValue()
count_1e1 = df_training_data.Filter("truDetRatioReweight < 1E1").Count().GetValue()

percentage = lambda sample, total : 100*(sample/total)

perecentage_removed = lambda sample,total : 100*(total-sample)/total

print("The size of the original training set is: ", df_size)

print("The number of points lying below 1E4 is ", count_1e4, "this is ", percentage(count_1e4, df_size), 
        "of the dataset, so we have removed ", perecentage_removed(count_1e4, df_size), "%")

print("The number of points lying below 1E3 is ", count_1e3, "this is ", percentage(count_1e3, df_size), 
        "of the dataset, so we have removed ", perecentage_removed(count_1e3, df_size), "%")

print("The number of points lying below 1E2 is ", count_1e2, "this is ", percentage(count_1e2, df_size), 
        "of the dataset, so we have removed ", perecentage_removed(count_1e2, df_size), "%")

print("The number of points lying below 1E1 is ", count_1e1, "this is ", percentage(count_1e1, df_size), 
        "of the dataset, so we have removed ", perecentage_removed(count_1e1, df_size), "%")

