close all;
clear;
addpath(genpath('Functions'))

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

camacho_good_carriers_heavy = zeros(num_loops,num_carriers,num_estimates);
camacho_good_carriers_medium = zeros(num_loops,num_carriers,num_estimates);
camacho_good_carriers_light = zeros(num_loops,num_carriers,num_estimates);

% harrison_good_carriers = zeros(num_loops,num_carriers,num_estimates);
% camacho_good_carriers = zeros(num_loops,num_carriers,num_estimates);
% chambers_good_carriers = zeros(num_loops,num_carriers,num_estimates);
% conference_good_carriers = zeros(num_loops,num_carriers,num_estimates);
% smalley_good_carriers = zeros(num_loops,num_carriers,num_estimates);
% harrison_sum_carriers = zeros(num_loops,num_estimates);
% camacho_sum_carriers = zeros(num_loops,num_estimates);
% chambers_sum_carriers = zeros(num_loops,num_estimates);
% conference_sum_carriers = zeros(num_loops,num_estimates);
% smalley_sum_carriers = zeros(num_loops,num_estimates);

cam_probabilities_heavy = zeros(num_loops,num_carriers);
cam_probabilities_medium = zeros(num_loops,num_carriers);
cam_probabilities_light = zeros(num_loops,num_carriers);

%     har_probabilities = zeros(num_loops,num_carriers);
%     cam_probabilities = zeros(num_loops,num_carriers);
%     cham_probabilities = zeros(num_loops,num_carriers);
%     conf_probabilities = zeros(num_loops,num_carriers);
%     smal_probabilities = zeros(num_loops,num_carriers);
% camacho_sum_carriers_heavy = zeros(num_loops,num_carriers);
% camacho_sum_carriers_medium = zeros(num_loops,num_carriers);
% camacho_sum_carriers_light = zeros(num_loops,num_carriers);

% eve = zeros(num_loops,num_estimates);
snr = logspace(1.8,2.2,num_loops); % the range of snr values to test
db_threshold = 0; % the threshold to test in db
threshold = 10^(db_threshold/10); % the threshold to test converted to linear
for index = 1 : num_loops
    [camacho_sum_carriers_heavy(index,:) ...
        ,camacho_good_carriers_heavy(index,:,:)] ...
        = find_num_carriers(camacho_heavy,snr(index),threshold);
    [camacho_sum_carriers_medium(index,:) ...
        ,camacho_good_carriers_medium(index,:,:)] ...
        = find_num_carriers(camacho_medium,snr(index),threshold);
    [camacho_sum_carriers_light(index,:) ...
        ,camacho_good_carriers_light(index,:,:)] ...
        = find_num_carriers(camacho_light,snr(index),threshold);
    %     [harrison_sum_carriers(index,:) ...
    %         ,harrison_good_carriers(index,:,:)] ...
    %         = find_num_carriers(harrison,snr(index),threshold);
    %     [camacho_sum_carriers(index,:) ...
    %         ,camacho_good_carriers(index,:,:)] ...
    %         = find_num_carriers(camacho,snr(index),threshold);
    %     [chambers_sum_carriers(index,:) ...
    %         ,chambers_good_carriers(index,:,:)] ...
    %         = find_num_carriers(chambers,snr(index),threshold);
    %     [conference_sum_carriers(index,:) ...
    %         ,conference_good_carriers(index,:,:)] ...
    %         = find_num_carriers(conference,snr(index),threshold);
    %     [smalley_sum_carriers(index,:) ...
    %         ,smalley_good_carriers(index,:,:)] ...
    %         = find_num_carriers(smalley,snr(index),threshold);
    
    figure();
    hold on;
    %     (histogram(harrison_sum_carriers(index,:)));
%     histogram(camacho_sum_carriers_heavy(index,:),'FaceColor','r','EdgeColor','r');
%     histogram(camacho_sum_carriers_medium(index,:),'FaceColor','b','EdgeColor','b');
%     histogram(camacho_sum_carriers_light(index,:),'FaceColor','y','EdgeColor','y');
    histogram(camacho_sum_carriers_heavy(index,:));
    histogram(camacho_sum_carriers_medium(index,:));
    histogram(camacho_sum_carriers_light(index,:));    
    %     histogram(chambers_sum_carriers(index,:));
    %     histogram(conference_sum_carriers(index,:));
    %     (histogram(smalley_sum_carriers(index,:)));
    xlabel('number of good carriers');
    ylabel('number of occurrences')
    legend('heavy','medium','light');
    grid on;
    hold off
    
    figure();
    hold on;
%     plot(harrison_sum_carriers(index,:),'b');
    plot(camacho_sum_carriers_heavy(index,:),'r');
    plot(camacho_sum_carriers_medium(index,:),'b');
    plot(camacho_sum_carriers_light(index,:),'y');
    xlabel('channel estimate number');
    ylabel('number of good carriers');
    legend('heavy','medium','light');

    %     plot(chambers_sum_carriers(index,:));
    %     plot(conference_sum_carriers(index,:));
%     plot(smalley_sum_carriers(index,:),'r');
    grid on;
    hold off
    
    %     har = sum((squeeze(harrison_good_carriers(index,:,:)).'));
%     har_probabilities(index,:) = sum((squeeze(harrison_good_carriers(index,:,:)).'))/4000;
    cam_probabilities_heavy(index,:) = sum((squeeze(camacho_good_carriers_heavy(index,:,:)).'))/40000;
    cam_probabilities_medium(index,:) = sum((squeeze(camacho_good_carriers_medium(index,:,:)).'))/40000;
    cam_probabilities_light(index,:) = sum((squeeze(camacho_good_carriers_light(index,:,:)).'))/40000;

    %     cham_probabilities(index,:) = (squeeze(chambers_good_carriers(index,:,:)).');
    %     conf_probabilities(index,:) = (squeeze(conference_good_carriers(index,:,:)).');
%     smal_probabilities(index,:) = sum((squeeze(smalley_good_carriers(index,:,:)).'))/4000;
    % %
    figure()
    hold on
    stem(cam_probabilities_heavy(index,:),'r');
    stem(cam_probabilities_medium(index,:),'b');
    stem(cam_probabilities_light(index,:),'y');
%     stem(har_probabilities(index,:));
%     stem(smal_probabilities(index,:));
    legend('heavy','medium','light');
    xlabel('carrier index');
    ylabel('probability of being good')
    grid on;
    hold off;
end
