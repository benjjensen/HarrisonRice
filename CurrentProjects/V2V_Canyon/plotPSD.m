% Plot PSD
close all; clear; clc;

load('Data/PSD/AlpineLoop_pt1A_PSD.mat');
AL_1A = outDataShifted;
load('Data/PSD/AlpineLoop_pt2A_PSD.mat')
AL_2A = outDataShifted;
load('Data/PSD/AlpineLoop_pt2B_PSD.mat')
AL_2B = outDataShifted;

load('Data/PSD/DiamondForkA_PSD.mat')
DF_A = outDataShifted;
load('Data/PSD/DiamondForkB_PSD.mat')
DF_B = outDataShifted;

figure()
subplot(3, 2, 1)
plot(AL_1A)
title('AL 1A')

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
