close all;

load("StationaryData/pwelch_harrison_array.mat");
load("StationaryData/pwelch_smalley_array.mat");
load("StationaryData/pwelch_camacho_array.mat");
load("StationaryData/pwelch_conference_array.mat");
load("StationaryData/pwelch_chambers_array.mat");

threshold = 25;

dist_harrison = find_num_carriers(pwelch_harrison_array,threshold);
dist_smalley = find_num_carriers(pwelch_smalley_array,threshold);
dist_camacho = find_num_carriers(pwelch_camacho_array,threshold);
dist_conference = find_num_carriers(pwelch_conference_array,threshold);
dist_chambers = find_num_carriers(pwelch_chambers_array,threshold);
figure(1);
hold on;
histogram(dist_harrison,'DisplayStyle','stairs');
histogram(dist_smalley,'DisplayStyle','stairs');
histogram(dist_camacho,'DisplayStyle','stairs');
histogram(dist_conference,'DisplayStyle','stairs');
histogram(dist_chambers,'DisplayStyle','stairs');
legend;
hold off;