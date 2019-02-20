% Graphs the number of carriers each room gets at each decibel level
    % Uses the best (max) location from each room 

%% Loads data from paper 1 (fourth floor of Clyde building)
close all;
load('CollectedData/chambers_pwelch.mat');
load('CollectedData/complete_pwelch.mat');
load('CollectedData/conference_pwelch.mat');
load('CollectedData/harrison_pwelch.mat');
load('CollectedData/smalley_pwelch.mat');

load('CollectedData/tx2_pwelch.mat');
load('CollectedData/tx2camacho_pwelch.mat');
load('CollectedData/tx2chambers_pwelch.mat');
load('CollectedData/tx2conference_pwelch.mat');
load('CollectedData/tx2harrison_pwelch.mat');
load('CollectedData/tx2smalley_pwelch.mat');
    
%% 
    % Number of Carriers above the dblimit (tx2)
stepSize = .1;
maxDB = 35;
har_carriersPerDB = carriersPerDB(tx2harrison_pwelch, stepSize, maxDB);
sma_carriersPerDB = carriersPerDB(tx2smalley_pwelch, stepSize, maxDB);
cam_carriersPerDB = carriersPerDB(tx2camacho_pwelch, stepSize, maxDB);
cha_carriersPerDB = carriersPerDB(tx2chambers_pwelch, stepSize, maxDB);
con_carriersPerDB = carriersPerDB(tx2conference_pwelch, stepSize, maxDB);

hold on
title({['\fontsize{12}Carriers Per DB'] ; ...
                ['\fontsize{11}Step Size = ' num2str(stepSize)]});
plot(har_carriersPerDB,'DisplayName','Harrison');
plot(sma_carriersPerDB,'DisplayName','Smalley');
plot(cam_carriersPerDB,'DisplayName','Camacho');
plot(cha_carriersPerDB,'DisplayName','Chambers');
plot(con_carriersPerDB,'DisplayName','Conference');
legend;
grid minor
hold off
