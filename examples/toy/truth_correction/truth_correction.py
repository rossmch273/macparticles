import os
import sys
os.environ["CUDA_VISIBLE_DEVICES"] = "-1"
import ROOT
from ROOT import RDataFrame
import numpy as np
import matplotlib.pyplot as plt
import tensorflow as tf
from tensorflow import keras
from keras import models
from keras import layers
from keras import callbacks

####################################################################################################
#COMMAND LINE INPUT VALIDATION

if len(sys.argv) <=1:
    quit()

particleName = str(sys.argv[1])



#change particle name to align with filenames better, if one of the correct particle names isn't entered quit
if particleName == "pi+":
    particleName = "pip"

elif particleName == "pi-":
    particleName = "pim"

elif particleName == "proton":
    particleName = "proton"

else:
    quit()

trainingFilePath = "correction-data/" + particleName + "-correction-training.root"
######################################################################################################

#READ data from root file into dataframe
df_training_data = RDataFrame("tree", trainingFilePath) #trainingFilePath

#extract specific columns we want for training purposes into dictionary of arrays
training_data = df_training_data.Filter("truDetRatioReweight < 1E2").AsNumpy(["detP", "detTheta","detPhi","truDetRatioReweight"])

#Dictionary keys for our features and our labels
feature_keys = ["detP", "detTheta","detPhi"]
label_key = "truDetRatioReweight"

#Extract arrays from dictionary and for features create multidimensional array
training_features = np.vstack([training_data[key] for key in feature_keys]).T
training_labels = np.array([training_data[label_key]]).T

print(training_labels.shape)
print(training_features.shape)


#Configure Neural Net
normalizer = keras.layers.Normalization(axis=-1)
normalizer.adapt(training_features)

sizes = [512, 128, 64, 16]

layer_list = [normalizer]
layer_list.extend([layers.Dense(size, activation='tanh') for size in sizes])
layer_list.append(layers.Dense(1))

dnn = keras.Sequential(layer_list)

#Train Model
dnn.compile(
    optimizer=keras.optimizers.Adam(learning_rate=1e-3),
    loss='mean_absolute_error'
    )

earlystopping = callbacks.EarlyStopping(monitor="loss", mode="min", patience=10, restore_best_weights=True)

history = dnn.fit(
    training_features,
    training_labels,
    batch_size = 1024,
    epochs = 100,
    callbacks = earlystopping
)

#Define file path to directory that we want to save DNN and error plots
dir_filepath = "DNNs/model7/" + particleName
plots_filepath = dir_filepath + "/plots"

#Check to see if directory we want to save to exists if not we create it
if not os.path.isdir(plots_filepath):
    os.makedirs(plots_filepath)

#Function to plot error on the neural net for performance
print('Saving plot of loss to : ' + plots_filepath + '/Error.png')
plt.figure()
plt.plot(history.history['loss'], label='loss')
plt.xlabel('Epoch')
plt.ylabel('Error [weight]')
plt.legend()
plt.grid(True)
plt.savefig(plots_filepath + '/Error.png')

#Save DNN
dnn_name = "correctionDNN"
print('Saving DNN to: ' + dir_filepath + "/" + dnn_name)
dnn.save(dir_filepath + '/' + dnn_name)

#Generate data for the dnn to predict on (this is only for the case where we remove statistical outliers from the training)
prediction_data = df_training_data.AsNumpy(["detP", "detTheta","detPhi","truDetRatioReweight"])
prediction_features = np.vstack([prediction_data[key] for key in feature_keys]).T
prediction_labels = np.array([prediction_data[label_key]]).T

#Generate Weights for the training data and store them with the labels for analysis
weight_ratios = dnn.predict(prediction_features) ##change this to training_features when we don't filter outliers

weightFileName = dir_filepath + "/corrections.root"

print("saving training weights to: ", weightFileName)

##change this to training_labels when we dont filter outliers
correction_df = ROOT.RDF.MakeNumpyDataFrame({"truth-correction":weight_ratios[:,0], "training-labels":prediction_labels[:,0]}) ##change this to training_labels when we dont filter outliers
correction_df.Snapshot("correctionDNN", weightFileName)




