%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%  Complementary CDF  %%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% IN TESTING - 12 NOV 2019   

%%%%% Overview %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Calculates the complementary cumulative distribution function:
%     1) Normalizes data by the maximum
%     2) Generates a histogram
%     3) Averages the histogram bins
%     4) Accumulates the data and take the complement (1 - data)
%
% Files Used:
% 
%   - powerPerMilliSecond.mat (produced in PowerPerTime.m)
% 
% Files Produced:
% 
%   - CCDF.mat
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

clear; close all; clc;

%%% User Input %%%
filename = 'Data/powerArray/DiamondForkB_powerArray.mat';
%%%%%%%%%%%%%%%%%%

load(filename);
% (1) Normalize by the maximum
% powerArray = powerArray(1:end-80); % Last little bit is bad... AL_1A
% powerArray = powerArray(1:end-8); % Last little bit is bad... AL_2A
% powerArray = powerArray(1:end); % Last little bit is bad... AL_2B
% powerArray = powerArray(1:end-80); % Last little bit is bad... DF_A
powerArray = powerArray(1:end-75); % Last little bit is bad... DF_B


normPowerArray = powerArray / max(powerArray);

% (2) Generate a histogram
figure()
y = histogram(10*log10(normPowerArray));
title('Histogram');
% xlim([-35, 0]);

% (3) Average the histogram bins
[~, edgeVals] = size(y.BinEdges);
for i = 1:(edgeVals - 1)
    xaxis(i) = mean(y.BinEdges(i:i+1));
end

yp = y.Values / length(normPowerArray);

% (4) Accumulate the values & take complement
cumulative = cumsum(yp);
cumuComp = 1 - cumulative(1:end - 1);

figure()
semilogy(xaxis(1:end-1), cumuComp);




