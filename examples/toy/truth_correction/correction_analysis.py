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


#FUNCTION DEFINITION FOR PLOTTING

def plot_scatter(x, y1, y2, x_label, y_label, xlim, ylim, plot_title, dir_filepath, filename):

    #check if the directory w want to save to dir_filepath exists
    if not os.path.isdir(dir_filepath):
        os.makedirs(dir_filepath)
    
    plt.figure()
    
    plt.subplot(2,1,1)
    plt.xlim(xmin=xlim[0], xmax=xlim[1])
    plt.ylim(ymin=ylim[0], ymax=ylim[1])
    plt.title(plot_title)
    plt.ylabel(y_label)
    plt.scatter(x,y1, marker = '.', s=0.05, label ='Data')
    plt.legend()

    plt.subplot(2,1,2)
    plt.xlim(xmin=xlim[0], xmax=xlim[1])
    plt.ylim(ymin=ylim[0], ymax=ylim[1])
    plt.ylabel(y_label)
    plt.xlabel(x_label)
    plt.scatter(x, y2, marker='.', s= 0.05, color='red', label='DNN')
    plt.legend()

    #save file
    plt.savefig(dir_filepath + '/' + filename)
    
def plot_heatmap(x, y, x_label, y_label, xlim, ylim, plot_title, dir_filepath, filename):

    #check if the directory we want to save to dir_filepath exists
    if not os.path.isdir(dir_filepath):
        os.makedirs(dir_filepath)

    #create limits array for 2d histogram    
    limits = np.array([xlim, ylim])

    plt.figure()
    plt.title(plot_title)
    plt.xlabel(x_label)
    plt.ylabel(y_label)
    plt.hist2d(x, y, bins=(200,200), range=limits, cmap='jet', norm=mpl.colors.LogNorm())
    plt.savefig(dir_filepath + '/' + filename)


mpl.rcParams['agg.path.chunksize'] = 10000
#mpl.rcParams['text.usetex'] = True

#MAIN CODE 

#Load DNN
model_dir = "DNNs/model6"
model_name = "correctionDNN"
dnn = keras.models.load_model(model_dir + '/' + model_name)

#Generate Training Data in the same way as in truth_correction.py
df_training_data = RDataFrame("tree", "correction-training.root")

#extract specific columns we want for training purposes into dictionary of arrays
training_data = df_training_data.Filter("truDetRatioReweight < 1E4").AsNumpy(["detP", "detTheta","detPhi","truDetRatioReweight"])

#Dictionary keys for our features and our labels
feature_keys = ["detP", "detTheta","detPhi"]
label_key = "truDetRatioReweight"

#Extract arrays from dictionary and for features create multidimensional array
training_features = np.vstack([training_data[key] for key in feature_keys]).T
training_labels = np.array([training_data[label_key]]).T

#Use DNN to predict the weight ratios for each p,theta,phi value
weight_ratios = dnn.predict(training_features)

#print(weight_ratios.shape)

print('The fast weight ratios we trained with: ',training_labels[:,0])
print('The neural nets prediction of the fast weight ratios: ',weight_ratios[:,0])



#SCATTER PLOTS

scatterDir_filepath = "DNNs/model6/scatter-plots"

plot_scatter(training_features[:,0], training_labels[:,0], weight_ratios[:,0], 'momentum P', 'tru/det weights', [0,10], [0,2], 
            'Momentum vs Weight Ratios', scatterDir_filepath, "momentum.png")

plot_scatter(training_features[:,1], training_labels[:,0], weight_ratios[:,0], 'Theta', 'tru/det weights', [0,2], [0,2], 
            'Theta vs Weight Ratios',  scatterDir_filepath, "theta.png")

plot_scatter(training_features[:,2], training_labels[:,0], weight_ratios[:,0], 'Phi', 'tru/det weights', [-4,4], [0,2], 
            'Theta vs Weight Ratios',  scatterDir_filepath, "phi.png")


#HEAT PLOTS
heatmapDir_filepath = "DNNs/model6/heatmaps"


plot_heatmap(training_features[:,0], weight_ratios[:,0], 'momentum P', 'tru/det weights DNN', [0,10], [0,2], 
            'Momentum vs Weight Ratios', heatmapDir_filepath, "momentumDNN.png")

plot_heatmap(training_features[:,1], weight_ratios[:,0], 'theta', 'tru/det weights DNN', [0,2], [0,2], 'theta vs Weight Ratios', 
            heatmapDir_filepath, "thetaDNN.png")

plot_heatmap(training_features[:,2], weight_ratios[:,0], 'phi', 'tru/det weights DNN', [-4,4], [0,2], 'phi vs Weight Ratios', 
            heatmapDir_filepath, "phiDNN.png")

plot_heatmap(training_features[:,0], training_labels[:,0], 'momentum P', 'tru/det weights', [0,10], [0,2], 
            'Momentum vs Weight Ratios', heatmapDir_filepath, "momentum.png")

plot_heatmap(training_features[:,1], training_labels[:,0], 'theta', 'tru/det weights', [0,2], [0,2], 'theta vs Weight Ratios', 
            heatmapDir_filepath, "theta.png")

plot_heatmap(training_features[:,2], training_labels[:,0], 'phi', 'tru/det weights', [-4,4], [0,2], 'phi vs Weight Ratios', 
            heatmapDir_filepath, "phi.png")

#Predictions vs Labels Plot
plt.figure()
plt.plot(np.log(training_labels[:100000,0]), np.log(weight_ratios[:100000,0]),'o')
plt.xlabel('labels')
plt.ylabel('predictions')
plt.title('fast-weights, labels vs predictions')
plt.savefig(scatterDir_filepath + '/labels-predictions.png')

plt.figure()
plt.hist(weight_ratios[:,0]-training_labels[:,0], bins=200, histtype='stepfilled')
plt.xlabel('predictions-labels')
plt.title('fast-weights, predictions-labels')
plt.savefig(scatterDir_filepath + '/labels-predictions-hist.png')

difference = np.absolute(weight_ratios[:,0] - training_labels[:,0])

print("the number of predictions with a difference >1 than the labels is ",np.count_nonzero(difference>1))
print("the number of predictions with a difference >10 than the labels is ",np.count_nonzero(difference>10))
print("the number of predictions with a difference >1000 than the labels is ",np.count_nonzero(difference>1000))

print("the maxmimum difference is ", np.amax(difference), "this occurs at index ", np.argmax(difference),
        "the label is ", training_labels[np.argmax(difference),0], "the prediction is ", 
        weight_ratios[np.argmax(difference),0])

print("max prediction: ", np.amax(weight_ratios[:,0]),"\n this occurs at index: ", 
        np.argmax(weight_ratios[:,0]),"\n the label for this is ", training_labels[np.argmax(weight_ratios[:,0]),0])

print("max label: ", np.amax(training_labels[:,0]),"\n this occurs at index: ", np.argmax(training_labels[:,0]),
        "\n the prediction for this is : ", weight_ratios[np.argmax(training_labels[:,0]),0])



