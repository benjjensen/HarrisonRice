close all;
clear;
addpath(genpath('Functions'))

%% Load in the data
load Data/LightMediumHeavy/Linear/camacho_heavy_linear;
load Data/LightMediumHeavy/Linear/camacho_medium_linear;
load Data/LightMediumHeavy/Linear/camacho_light_linear;
% load Data/LightMediumHeavy/Linear/chambers_heavy_linear;
% load Data/LightMediumHeavy/Linear/chambers_medium_linear;
% load Data/LightMediumHeavy/Linear/chambers_light_linear;
% load Data/LightMediumHeavy/Linear/conference_heavy_linear;
% load Data/LightMediumHeavy/Linear/conference_medium_linear;
% load Data/LightMediumHeavy/Linear/conference_light_linear;
% load Data/LightMediumHeavy/Linear/harrison_heavy_linear;
% load Data/LightMediumHeavy/Linear/harrison_medium_linear;
% load Data/LightMediumHeavy/Linear/harrison_light_linear;
% load Data/LightMediumHeavy/Linear/smalley_heavy_linear;
% load Data/LightMediumHeavy/Linear/smalley_medium_linear;
% load Data/LightMediumHeavy/Linear/smalley_light_linear;

%% Remove the bad carriers
% harrison_heavy = harrison_heavy_linear(22:2:110,:);
% harrison_medium = harrison_medium_linear(22:2:110,:);
% harrison_light = harrison_light_linear(22:2:110,:);
camacho_heavy = camacho_heavy_linear(22:2:110,:);
camacho_medium = camacho_medium_linear(22:2:110,:);
camacho_light = camacho_light_linear(22:2:110,:);
% chambers_heavy = camacho_heavy_linear(22:2:110,:);
% chambers_medium = camacho_medium_linear(22:2:110,:);
% chambers_light = camacho_light_linear(22:2:110,:);
% conference_heavy = conference_heavy_linear(22:2:110,:);
% conference_medium = conference_medium_linear(22:2:110,:);
% conference_light = conference_light_linear(22:2:110,:);
% smalley_heavy = smalley_heavy_linear(22:2:110,:);
% smalley_medium = smalley_medium_linear(22:2:110,:);
% smalley_light = smalley_light_linear(22:2:110,:);

%% Normalize the signals
% % % [camacho,~] = normalize_noise(camacho_pwelch);
% % % camacho = (sqrt(camacho)./normalization_factor).^2;
% % % [chambers,~] = normalize_noise(chambers_pwelch);
% % % chambers = (sqrt(chambers)./normalization_factor).^2;
% % % [conference,~] = normalize_noise(conference_pwelch);
% % % conference = (sqrt(conference)./normalization_factor).^2;
% % % [smalley,~] = normalize_noise(smalley_pwelch);
% % % smalley = (sqrt(smalley)./normalization_factor).^2;


num_loops = 15;
num_estimates = 40000;
num_carriers = 45;

%% Initialize matrices
camacho_good_carriers_heavy = zeros(num_loops,num_carriers,num_estimates);
camacho_good_carriers_medium = zeros(num_loops,num_carriers,num_estimates);
camacho_good_carriers_light = zeros(num_loops,num_carriers,num_estimates);

cam_probabilities_heavy = zeros(num_loops,num_carriers);
cam_probabilities_medium = zeros(num_loops,num_carriers);
cam_probabilities_light = zeros(num_loops,num_carriers);

snr = logspace(1.8,2.2,num_loops); % the range of snr values to test
db_threshold = 0; % the threshold to test in db
threshold = 10^(db_threshold/10); % the threshold to test converted to linear
%% analyze different snr levels
for index = 1 : num_loops
    %% find the number of good carriers and which carriers are good
    [camacho_sum_carriers_heavy(index,:) ...
        ,camacho_good_carriers_heavy(index,:,:)] ...
        = find_num_carriers(camacho_heavy,snr(index),threshold);
    [camacho_sum_carriers_medium(index,:) ...
        ,camacho_good_carriers_medium(index,:,:)] ...
        = find_num_carriers(camacho_medium,snr(index),threshold);
    [camacho_sum_carriers_light(index,:) ...
        ,camacho_good_carriers_light(index,:,:)] ...
        = find_num_carriers(camacho_light,snr(index),threshold);
    
    %% plot a histogram of the frequency of the number of good carriers
    figure();
    hold on;
    histogram(camacho_sum_carriers_heavy(index,:));
    histogram(camacho_sum_carriers_medium(index,:));
    histogram(camacho_sum_carriers_light(index,:));    
    xlabel('number of good carriers');
    ylabel('number of occurrences')
    legend('heavy','medium','light');
    grid on;
    hold off
    
    %% plot the number of good carriers with respect to the estimate number
    figure();
    hold on;
    plot(camacho_sum_carriers_heavy(index,:),'r');
    plot(camacho_sum_carriers_medium(index,:),'b');
    plot(camacho_sum_carriers_light(index,:),'y');
    xlabel('channel estimate number');
    ylabel('number of good carriers');
    legend('heavy','medium','light');
    grid on;
    hold off
    
    %% calculate the carrier probabilities
    cam_probabilities_heavy(index,:) = sum((squeeze(camacho_good_carriers_heavy(index,:,:)).'))/40000;
    cam_probabilities_medium(index,:) = sum((squeeze(camacho_good_carriers_medium(index,:,:)).'))/40000;
    cam_probabilities_light(index,:) = sum((squeeze(camacho_good_carriers_light(index,:,:)).'))/40000;

    %% plot the carrier probabilities
    figure()
    hold on
    stem(cam_probabilities_heavy(index,:),'r');
    stem(cam_probabilities_medium(index,:),'b');
    stem(cam_probabilities_light(index,:),'y');
    legend('heavy','medium','light');
    xlabel('carrier index');
    ylabel('probability of being good')
    grid on;
    hold off;
end
