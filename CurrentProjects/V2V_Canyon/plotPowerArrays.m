% Plot PSD
close all; clear; clc;

load('Data/powerArray/AlpineLoop_pt1A_powerArray.mat');
AL_1A = powerArray;
load('Data/powerArray/AlpineLoop_pt2A_powerArray.mat');
AL_2A =  powerArray;
load('Data/powerArray/AlpineLoop_pt2B_powerArray.mat');
AL_2B =  powerArray;

load('Data/powerArray/DiamondForkA_powerArray.mat');
DF_A =  powerArray;
load('Data/powerArray/DiamondForkB_powerArray.mat');
DF_B =  powerArray;

figure()
subplot(3, 2, 1)
plot(AL_1A)
title('AL 1A')
xlabel('time (ms)')
ylabel('

subplot(3,2,2)
plot(AL_2A)
title('AL 2A')

subplot(3,2,3)
plot(AL_2B)
title('AL 2B')

subplot(3,2,4)
plot(DF_A)
title('DF A')

subplot(3,2,5)
plot(DF_B)
title('DF B')
