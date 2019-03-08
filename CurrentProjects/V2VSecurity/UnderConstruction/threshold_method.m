% Created by Dakota Flanary
%% Plots the number of good carriers and unique good carriers for harrison
%% at each SNR level
clear;
close all;
load('../Data/GraphPwelchedData/test-point-A_graph-pwelched-data.mat'); % loads in the signal data
alpha = sqrt(graphPwelchedData);
load('../Data/GraphPwelchedData/test-point-F_graph-pwelched-data.mat'); % loads in the signal data
foxtrot = graphPwelchedData;
load('../Data/GraphPwelchedData/test-point-G_graph-pwelched-data.mat'); % loads in the signal data
golf = graphPwelchedData;
load('../Data/GraphPwelchedData/test-point-H_graph-pwelched-data.mat'); % loads in the signal data
hotel = graphPwelchedData;
load('../Data/GraphPwelchedData/test-point-I_graph-pwelched-data.mat'); % loads in the signal data
india = graphPwelchedData;
load('../Data/GraphPwelchedData/test-point-J_graph-pwelched-data.mat'); % loads in the signal data
juliet = graphPwelchedData;
clear graphPwelchedData;
alpha_max = max(max(alpha));
alpha = (alpha/alpha_max).^2;
fox_max = max(max(foxtrot));
golf_max = max(max(golf));
hotel_max = max(max(hotel));
india_max = max(max(india));
juliet_max = max(max(juliet));
normalize_max = sqrt(max([fox_max,golf_max,hotel_max,india_max,juliet_max]));
foxtrot = (sqrt(foxtrot)/normalize_max).^2;
golf = (sqrt(golf)/normalize_max).^2;
hotel = (sqrt(hotel)/normalize_max).^2;
india = (sqrt(india)/normalize_max).^2;
juliet = (sqrt(juliet)/normalize_max).^2;
addpath(genpath('Functions'))

[v2i_num_locations,v2i_num_carriers] = size(alpha);
[v2v_num_locations,v2v_num_carriers] = size(foxtrot);


num_loops = 50; % number of snr values to test
cap_alpha = zeros(v2i_num_locations,num_loops);
cap_foxtrot = zeros(v2v_num_locations,num_loops);
cap_golf = zeros(v2v_num_locations,num_loops); % capcity in camcho's office
cap_hotel = zeros(v2v_num_locations,num_loops); % capacity in chamber's office
cap_india = zeros(v2v_num_locations,num_loops); % capacity in the conference room
cap_juliet = zeros(v2v_num_locations,num_loops); % capacity in harrison's office

snr = logspace(-0,3,num_loops); % snr values to test(in linear)
db_threshold = 0; % the threshold to test in db
threshold = 10^(db_threshold/10); % the threshold to test converted to linear
for index = 1:num_loops
    % Find the number of carriers(capacity) in each office at each location
    cap_foxtrot(:,index) = find_num_carriers(foxtrot,snr(index),threshold);
    cap_golf(:,index) = find_num_carriers(golf,snr(index),threshold);
    cap_hotel(:,index) = find_num_carriers(hotel,snr(index),threshold);
    cap_india(:,index) = find_num_carriers(india,snr(index),threshold);
    cap_juliet(:,index) = find_num_carriers(juliet,snr(index),threshold);
    
    figure()
    hold on
    plot(cap_foxtrot(:,index));
    plot(cap_golf(:,index));
    plot(cap_hotel(:,index));
    plot(cap_india(:,index));
    plot(cap_juliet(:,index));
    
    hold off
end

v2i_snr = logspace(3.5,5,num_loops);
for index = 1:num_loops
    cap_alpha(:,index) = find_num_carriers(alpha,v2i_snr(index),threshold);
    figure()
    hold on
    plot(cap_alpha(:,index));
    
    hold off
end
%%

% % plot the capacities and secrecy capacities
% snr = 10*log10(snr);
% figure()
% hold on;
% plot(snr,cap_har,'DisplayName','Bob');
% plot(snr,eve,'DisplayName','Eve');
% plot(snr,sec_har, 'DisplayName', 'Secrecy Capacity');
% xlabel('SNR (dB)');
% ylabel('Bits per channel use');
% legend;
% hold off;
