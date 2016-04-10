%% svm2k run
clear;
clc;
%% load all the data
% folder = 'k_n/';
folder = 'kxm/';
% file2D = [folder 'k_n.2df_c'];
file2D = 'e:\\data\\train.2dfc';
% file3D = [folder 'k_n.3df_c'];
file3D = 'e:\\data\\train.3dfc';
% fileLabel = [folder 'h005_k_n.label_c'];
fileLabel = 'e:\\data\\train.labelc';
fea2D = load(file2D);
fea3D = load(file3D);
label = load(fileLabel);
% label = (label+1)/2;

fea2D = fea2D';
fea3D = fea3D';

fea2D = mapminmax(fea2D);
fea3D = mapminmax(fea3D);

ncases = size(fea2D,2);
%% add code for ten fold
preLabel = zeros(ncases,1);
%% use one part for test
auc = [];
for fold = 1:5
%         fold = 1;
        % id for all
        id = 1:ncases;
        % testId
        testId = fold:5:ncases;
        % others are trainId
        trainId = setdiff(id,testId);
        XTrain1 = fea2D(:,trainId);
        XTrain2 = fea3D(:,trainId);
        XTrain1 = XTrain1';
        XTrain2 = XTrain2';
        YTrain = label(trainId);
        XTest1 = fea2D(:,testId);
        XTest2 = fea3D(:,testId);
        
        XTest2 = XTest2';
        XTest1 = zeros(size(XTest2, 1), size(XTrain1, 2));
        YTest = label(testId);
        CA = 0.2;
        CB = 0.2;
        D = 0.1;
        eps = 0.001;
        % this parameter about kernel
        ifeature = 1;
        [acorr,acorr1,acorr2,pre,pre1,pre2,hit,hit1,hit2,tpre,tpre1,tpre2,ga,gb,bam,bbm,alpha_A,alpha_B]= ...
            mc_svm_2k_lava2(XTrain1,XTrain2,YTrain,XTest1,XTest2,YTest,CA,CB,D,eps,ifeature);
    preLabel(testId) = pre; 
     [X,Y,T,AUC] = perfcurve(YTest,pre,1);
      disp('AUC')
      disp(AUC)
      auc = [auc AUC];
      save(['e:\\data\\pre.' num2str(fold)], 'pre', '-ascii');
      save(['e:\\data\\label.'  num2str(fold)], 'YTest', '-ascii');
 end %for fold = 1:10
% disp(preLabel)
disp('mean auc')
disp(mean(auc))
%  disp('AUC')
%  disp(AUC)
%  plot(X,Y)
plot(X,Y)
xlabel('False positive rate'); ylabel('True positive rate')
title('ROC for classification by logistic regression') 

