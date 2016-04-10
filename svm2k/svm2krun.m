%% svm2k run
clear;
clc;
%% load all the data
fea2D = load('e:\\data\\train.2dfc');
fea3D = load('e:\\data\\train.3dfc');
test3D = load('e:\\data\\test.3dfc');
trainlabel = load('e:\\data\\train.labelc');
testlabel = load('e:\\data\\test.labelc');
% label = (label+1)/2;

fea2D = fea2D';
fea3D = fea3D';
test3D = test3D';
fea2D = mapminmax(fea2D);
[fea3D, fea3DPS] = mapminmax(fea3D);
test3D = mapminmax('apply', test3D, fea3DPS); 
fea2D = fea2D';
fea3D = fea3D';
test3D = test3D';

XTrain1 = fea2D;
XTrain2 = fea3D;
YTrain = trainlabel;

XTest1 = zeros(size(test3D, 1), size(XTrain1, 2));
XTest2 = test3D;
YTest = testlabel;

CA = 0.2;
CB = 0.2;
D = 0.1;
eps = 0.001;
% this parameter about kernel
ifeature = 1;
[acorr,acorr1,acorr2,pre,pre1,pre2,hit,hit1,hit2,tpre,tpre1,tpre2,ga,gb,bam,bbm,alpha_A,alpha_B]= ...
    mc_svm_2k_lava2(XTrain1,XTrain2,YTrain,XTest1,XTest2,YTest,CA,CB,D,eps,ifeature);

[X,Y,T,AUC] = perfcurve(YTest,pre,1);

AUC
