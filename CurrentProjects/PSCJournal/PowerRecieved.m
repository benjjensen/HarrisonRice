% Power Received 

% clear; close all;

stepSize = 100; % Milliseconds
    
% load('powerArray.mat')

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
xlim([0, 300000]);
ylim([0, 15e6]);

figure()
plot(powerArray)
xlim([0,226000]);
ylim([0, 10e5]);


