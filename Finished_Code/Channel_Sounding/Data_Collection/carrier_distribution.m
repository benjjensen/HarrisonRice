close all;

pwelched_measurements = [];
load("StationaryData/pwelch_harrison_array.mat", "pwelched_measurements");
pwelch_harrison_array = pwelched_measurements;
load("StationaryData/pwelch_smalley_array.mat", "pwelched_measurements");
pwelch_smalley_array = pwelched_measurements;
load("StationaryData/pwelch_camacho_array.mat", "pwelched_measurements");
pwelch_camacho_array = pwelched_measurements;
load("StationaryData/pwelch_conference_array.mat", "pwelched_measurements");
pwelch_conference_array = pwelched_measurements;
load("StationaryData/pwelch_chambers_array.mat", "pwelched_measurements");
pwelch_chambers_array = pwelched_measurements;

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

carrier_harrison = find_best_carriers(pwelch_harrison_array, threshold);
carrier_smalley = find_best_carriers(pwelch_smalley_array, threshold);
carrier_camacho = find_best_carriers(pwelch_camacho_array, threshold);
carrier_conference = find_best_carriers(pwelch_conference_array, threshold);
carrier_chambers = find_best_carriers(pwelch_chambers_array, threshold);


figure(2)
hold on;
% for carrier = 1:64
% 
    histogram(carrier_harrison,'DisplayStyle','stairs','BinWidth',1);
    histogram(carrier_smalley,'DisplayStyle','stairs','BinWidth',1);
    histogram(carrier_camacho,'DisplayStyle','stairs','BinWidth',1);
    histogram(carrier_conference,'DisplayStyle','stairs','BinWidth',1);
    histogram(carrier_chambers,'DisplayStyle','stairs','BinWidth',1);

% end

ylim([0,4000]);

legend;
hold off;

