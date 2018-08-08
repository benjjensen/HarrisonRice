close all;
tic

% Loads the data needed into the workspace
load("StationaryData/pwelch_harrison_array.mat");
load("StationaryData/pwelch_smalley_array.mat");
load("StationaryData/pwelch_camacho_array.mat");
load("StationaryData/pwelch_conference_array.mat");
load("StationaryData/pwelch_chambers_array.mat");

total = 71;
% this number is based off of the length of the thresholds we're looking at
% so starting at 25 and incrementing every .1 up to 32 there are 71 steps
% so our vectors need to that many indices in the first dimension
% Initializes the vectors to save time 
harrison_carriers = zeros(total,4000,1);
smalley_carriers = zeros(total,4000,1);
camacho_carriers = zeros(total,4000,1);
conference_carriers = zeros(total,4000,1);
chambers_carriers = zeros(total,4000,1);
spot = 0;

% Finds the number of carriers above the threshold for every threshold and
% all 4000 samples
for threshold = 25:.1:32
    spot = spot + 1;
    harrison_carriers(spot,:,:) = find_num_carriers(pwelch_harrison_array,threshold);
    smalley_carriers(spot,:,:) = find_num_carriers(pwelch_smalley_array,threshold);
    camacho_carriers(spot,:,:) = find_num_carriers(pwelch_camacho_array,threshold);
    conference_carriers(spot,:,:) = find_num_carriers(pwelch_conference_array,threshold);
    chambers_carriers(spot,:,:) = find_num_carriers(pwelch_chambers_array,threshold);
end

% intializes
average_harrison = zeros(total,1);
average_smalley = zeros(total,1);
average_camacho = zeros(total,1);
average_conference = zeros(total,1);
average_chambers = zeros(total,1);
std_harrison = zeros(total,1);
std_smalley = zeros(total,1);
std_camacho = zeros(total,1);
std_conference = zeros(total,1);
std_chambers = zeros(total,1);

for spot = 1:total
    
    average_harrison(spot,:) = mean(harrison_carriers(spot,:));
    average_smalley(spot,:) = mean(smalley_carriers(spot,:));
    average_camacho(spot,:) = mean(camacho_carriers(spot,:));
    average_conference(spot,:) = mean(conference_carriers(spot,:));
    average_chambers(spot,:) = mean(chambers_carriers(spot,:));
    std_harrison(spot,:) = std(harrison_carriers(spot,:));
    std_smalley(spot,:) = std(smalley_carriers(spot,:));
    std_camacho(spot,:) = std(camacho_carriers(spot,:));
    std_conference(spot,:) = std(conference_carriers(spot,:));
    std_chambers(spot,:) = std(chambers_carriers(spot,:));
end

% figure(1)
% hold on;
% histogram(average_harrison);
% histogram(average_smalley);
% histogram(average_camacho);
% histogram(average_conference);
% histogram(average_chambers);
% legend;
% hold off
x = 25:.1:32;
figure(1)
hold on;
plot(x,average_harrison);
plot(x,average_smalley);
plot(x,average_camacho);
plot(x,average_conference);
plot(x,average_chambers);
xlabel('Thresholds (dB)');
ylabel('Average number of carriers');
legend('Harrison','Smalley','Camacho','Conference','Chambers');
hold off
x = 1:64;
for spot = 1:total
    figure(spot+1)
    hold on;
    har = normpdf(x,average_harrison(spot,1),std_harrison(spot,1));
    smal = normpdf(x,average_smalley(spot,1),std_smalley(spot,1));
    cam = normpdf(x,average_camacho(spot,1),std_camacho(spot,1));
    conf = normpdf(x,average_conference(spot,1),std_conference(spot,1));
    cham = normpdf(x,average_chambers(spot,1),std_chambers(spot,1));
    plot(x,har);
    plot(x,smal);
    plot(x,cam);
    plot(x,conf);
    plot(x,cham);
    xlabel('Number of carriers');
    legend('Harrison','Smalley','Camacho','Conference','Chambers');

end
toc