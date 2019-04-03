%% Plots the number of good carriers and unique good carriers for harrison
%% at each SNR level
%% Created by Dakota Flanary

clear;
% close all;
load('../Data/CorrectlyAveragedData/test-point-A_jagged-mid32.mat'); % loads in the signal data
alpha = sqrt(pwelched);
load('../Data/CorrectlyAveragedData/test-point-F_average41-mid32.mat'); % loads in the signal data
foxtrot = pwelched;
load('../Data/CorrectlyAveragedData/test-point-G_average41-mid32.mat'); % loads in the signal data
golf = pwelched;
load('../Data/CorrectlyAveragedData/test-point-H_average41-mid32.mat'); % loads in the signal data
hotel = pwelched;
load('../Data/CorrectlyAveragedData/test-point-I_average41-mid32.mat'); % loads in the signal data
india = pwelched;
load('../Data/CorrectlyAveragedData/test-point-J_average41-mid32.mat'); % loads in the signal data
juliet = pwelched;
clear pwelched;

% change to true if you want to display figures
iWantFigures = true;

alpha_max = max(max(alpha));
alpha = (alpha/alpha_max).^2; %normalize the alpha (v2i) run

% find the max from each run
fox_max = max(max(foxtrot));
golf_max = max(max(golf));
hotel_max = max(max(hotel));
india_max = max(max(india));
juliet_max = max(max(juliet));

%normalize all of the v2v runs based on the overall max
normalize_max = sqrt(max([fox_max,golf_max,hotel_max,india_max,juliet_max]));
foxtrot = (sqrt(foxtrot)/normalize_max).^2;
golf = (sqrt(golf)/normalize_max).^2;
hotel = (sqrt(hotel)/normalize_max).^2;
india = (sqrt(india)/normalize_max).^2;
juliet = (sqrt(juliet)/normalize_max).^2; 

addpath(genpath('Functions')) % add the functions to this path

[v2i_num_locations,v2i_num_carriers] = size(alpha); % get the size of the v2i arrays
[v2v_num_locations,v2v_num_carriers] = size(foxtrot); % get the size of the v2v arrays


num_loops = 50; % number of snr values to test
cap_alpha = zeros(v2i_num_locations,num_loops); % capacity from the alpha run
cap_foxtrot = zeros(v2v_num_locations,num_loops); % capacity from the foxtrot run
cap_golf = zeros(v2v_num_locations,num_loops); % capacity from the golf run
cap_hotel = zeros(v2v_num_locations,num_loops); % capacity from the hotel run
cap_india = zeros(v2v_num_locations,num_loops); % capacity from the india run
cap_juliet = zeros(v2v_num_locations,num_loops); % capacity from the juliet run

snr = logspace(-0,5,num_loops); % snr values to test(in linear)
db_threshold = 0; % the threshold to test in db
threshold = 10^(db_threshold/10); % the threshold to test converted to linear

% calculate the capacities at different SNR values for each v2v run
for index = 1:num_loops
    % Find the number of carriers(capacity) of each run at every location
    cap_foxtrot(:,index) = find_num_carriers(foxtrot,snr(index),threshold);
    cap_golf(:,index) = find_num_carriers(golf,snr(index),threshold);
    cap_hotel(:,index) = find_num_carriers(hotel,snr(index),threshold);
    cap_india(:,index) = find_num_carriers(india,snr(index),threshold);
    cap_juliet(:,index) = find_num_carriers(juliet,snr(index),threshold);
    
    % code for displaying figures of the capacities
    if (iWantFigures)
        figure()
        hold on
        plot(cap_foxtrot(:,index));
        plot(cap_golf(:,index));
        plot(cap_hotel(:,index));
        plot(cap_india(:,index));
        plot(cap_juliet(:,index));
        hold off
    end
end
num_loops_carriers_per_location = 50;
v2i_snr = logspace(2,3,num_loops_carriers_per_location);

% calculate the capacity for the v2i run at different SNR values 
for index = 1:num_loops_carriers_per_location
    cap_alpha(:,index) = find_num_carriers(alpha,v2i_snr(index),threshold);
    
    % code for displaying a figure for each SNR value
    if (iWantFigures)
        figure()
        hold on
        plot(cap_alpha(:,index));
        hold off
    end
end

<<<<<<< HEAD:CurrentProjects/V2VSecurity/UnderConstruction/threshold_method.m
save('../Data/dataForCodedCases41-correct.mat', 'cap_alpha', 'cap_foxtrot', 'cap_golf', ...
    'cap_hotel', 'cap_india', 'cap_juliet', 'snr', 'v2i_snr');

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
=======
>>>>>>> a1059f0b845fad8bcbdf5cbbfd7318be7fb30dad:CurrentProjects/V2VSecurity/threshold_method.m
