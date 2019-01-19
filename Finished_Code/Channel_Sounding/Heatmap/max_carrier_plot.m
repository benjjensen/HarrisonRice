%% Plots the number of good carriers at each DB threshold

close all;

% load('camacho.mat');
% load('chambers.mat');
% load('conference.mat');
% load('hall_pwelch.mat');
% load('harrison.mat');
% load('smalley.mat');
load('tx2camacho.mat');
load('tx2chambers.mat');
load('tx2conference.mat');
load('tx2hallway.mat');
load('tx2harrison.mat');
load('tx2smalley.mat');

tx2harrison = sqrt(10.^(tx2harrison/10));
rmax = max(max(max(tx2harrison)));
tx2harrison = (tx2harrison/rmax).^2;
tx2camacho = (sqrt(10.^(tx2camacho/10))/rmax).^2;
tx2chambers = (sqrt(10.^(tx2chambers/10))/rmax).^2;
tx2conference = (sqrt(10.^(tx2conference/10))/rmax).^2;
tx2csmalley = (sqrt(10.^(tx2smalley/10))/rmax).^2;


mm_cam = zeros(351,1);
mm_cham = zeros(351,1);
mm_conf = zeros(351,1);
mm_har = zeros(351,1);
mm_smal = zeros(351,1);
mum_har = zeros(351,1);
i = 0;
num_loops = 10000;
snr = logspace(0,3,num_loops);
threshold = 10^(5/10);
% for threshold = 0:.1:35
for index = 1:num_loops
    i = i + 1;
    cam2 = find_num_carriers(tx2camacho,snr(index),threshold);
    chambers2 = find_num_carriers(tx2chambers,snr(index),threshold);
    conference2 = find_num_carriers(tx2conference,snr(index),threshold);
    harrison2 = find_num_carriers(tx2harrison,snr(index),threshold);
    smalley2 = find_num_carriers(tx2smalley,snr(index),threshold);
    
    % uncomment to find the carriers from the first transmitter location
%     camacho_best_carrier = find_best_carriers(cam1,camacho,3,threshold);
    camacho_best_carrier2 = find_best_carriers(cam2,tx2camacho,3,snr(index),threshold);
%     chambers_best_carrier = find_best_carriers(chambers1,chambers,2,threshold);
    chambers_best_carrier2 = find_best_carriers(chambers2,tx2chambers,2,snr(index),threshold);
%     conference_best_carrier = find_best_carriers(conference1,conference,1,threshold);
    conference_best_carrier2 = find_best_carriers(conference2,tx2conference,1,snr(index),threshold);
%     harrison_best_carrier = find_best_carriers(harrison1,harrison,5,threshold);
    harrison_best_carrier2 = find_best_carriers(harrison2,tx2harrison,5,snr(index),threshold);
%     smalley_best_carrier = find_best_carriers(smalley1,smalley,4,threshold);
    smalley_best_carrier2 = find_best_carriers(smalley2,tx2smalley,4,snr(index),threshold);

    [mm_cam(i,1),mm_cham(i,1),mm_conf(i,1),...
        mm_har(i,1),mm_smal(i,1),mum_har(i,1)] = find_max_matched(...
        camacho_best_carrier2,chambers_best_carrier2,...
        conference_best_carrier2,harrison_best_carrier2,...
        smalley_best_carrier2);
    
end
%%
snr = 10*log10(snr);
figure(1)
hold on;
plot(snr,mm_cam,'DisplayName','Camacho');
plot(snr,mm_cham,'DisplayName','Chambers');
plot(snr,mm_conf,'DisplayName','Conference Room');
plot(snr,mm_har,'DisplayName','Harrison');
plot(snr,mm_smal,'DisplayName','Smalley');
plot(snr,mum_har, 'DisplayName', 'Unique Harrison Carriers');
xlabel('DB Threshold');
ylabel('Number of Carriers');
% xlim(x);
legend;
hold off;
