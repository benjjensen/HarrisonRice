% Power Received 

% Load in the powerArray array
% Average over every (100 ms)    -> (cant this be done with a convolution?)
    % Average in linear
    % Plot (in dB)

clear; close all;

stepSize = 100; % Milliseconds
    
    
load('powerArray.mat')

dataCount = length(powerArray);
averages = zeros(ceil(dataCount/stepSize),1);
saveIndex = 1;

for index = 1:(stepSize-1):(dataCount-stepSize)
    averages(saveIndex,1) = mean(powerArray(1,index:(index+stepSize-1)));
    saveIndex = saveIndex + 1;
end

%
%%

figure()
plot(averages)
xlim([0, 2260]);
ylim([0, 10e5]);

figure()
plot(powerArray)
xlim([0,226000]);
ylim([0, 10e5]);


