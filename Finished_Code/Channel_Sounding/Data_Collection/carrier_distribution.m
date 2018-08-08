close all;

load("StationaryData/pwelch_harrison_array.mat");
load("StationaryData/pwelch_smalley_array.mat");
load("StationaryData/pwelch_camacho_array.mat");
load("StationaryData/pwelch_conference_array.mat");
load("StationaryData/pwelch_chambers_array.mat");
spot = 0;
for threshold = 25:.1:32
    spot = spot + 1;
    dist_harrison = find_num_carriers(pwelch_harrison_array,threshold);
    dist_smalley = find_num_carriers(pwelch_smalley_array,threshold);
    dist_camacho = find_num_carriers(pwelch_camacho_array,threshold);
    dist_conference = find_num_carriers(pwelch_conference_array,threshold);
    dist_chambers = find_num_carriers(pwelch_chambers_array,threshold);
    figure(spot);
    hold on;
    histogram(dist_harrison,'DisplayStyle','stairs');
    histogram(dist_smalley,'DisplayStyle','stairs');
    histogram(dist_camacho,'DisplayStyle','stairs');
    histogram(dist_conference,'DisplayStyle','stairs');
    histogram(dist_chambers,'DisplayStyle','stairs');
    legend;
    hold off;
end

for threshold = 25:.1:32

    spot = spot + 1;
    
    carrier_harrison = find_best_carriers(pwelch_harrison_array, threshold);
    carrier_smalley = find_best_carriers(pwelch_smalley_array, threshold);
    carrier_camacho = find_best_carriers(pwelch_camacho_array, threshold);
    carrier_conference = find_best_carriers(pwelch_conference_array, threshold);
    carrier_chambers = find_best_carriers(pwelch_chambers_array, threshold);


    figure(spot)
    hold on;
    % for carrier = 1:64
    % 
        histogram(carrier_harrison,'DisplayStyle','stairs','BinMethod','integers');
        histogram(carrier_smalley,'DisplayStyle','stairs','BinMethod','integers');
        histogram(carrier_camacho,'DisplayStyle','stairs','BinMethod','integers');
        histogram(carrier_conference,'DisplayStyle','stairs','BinMethod','integers');
        histogram(carrier_chambers,'DisplayStyle','stairs','BinMethod','integers');

    % end

    %ylim([0,4000]);

    legend;
    hold off;
end
