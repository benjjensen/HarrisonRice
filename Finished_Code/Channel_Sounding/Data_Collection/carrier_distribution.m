close all;

load("StationaryData/pwelch_harrison_array.mat");
load("StationaryData/pwelch_smalley_array.mat");
load("StationaryData/pwelch_camacho_array.mat");
load("StationaryData/pwelch_conference_array.mat");
load("StationaryData/pwelch_chambers_array.mat");
spot = 0;
% for threshold = 25:.1:32
%     spot = spot + 1;
%     dist_harrison = find_num_carriers(pwelch_harrison_array,threshold);
%     dist_smalley = find_num_carriers(pwelch_smalley_array,threshold);
%     dist_camacho = find_num_carriers(pwelch_camacho_array,threshold);
%     dist_conference = find_num_carriers(pwelch_conference_array,threshold);
%     dist_chambers = find_num_carriers(pwelch_chambers_array,threshold);
%     figure(spot);
%     hold on;
%     histogram(dist_harrison,'DisplayStyle','stairs');
%     histogram(dist_smalley,'DisplayStyle','stairs');
%     histogram(dist_camacho,'DisplayStyle','stairs');
%     histogram(dist_conference,'DisplayStyle','stairs');
%     histogram(dist_chambers,'DisplayStyle','stairs');
%     legend('Harrison','Smalley','Camacho','Conference','Chambers');
%     hold off;
% end
carrier_harrison = zeros(71,64,4000);
carrier_smalley = zeros(71,64,4000);
carrier_camacho = zeros(71,64,4000);
carrier_conference = zeros(71,64,4000);
carrier_chambers = zeros(71,64,4000);
index = 0;
for threshold = 25:.1:32
    index = index + 1;
    spot = spot + 1;
    
    carrier_harrison(index,:,:) = find_best_carriers(pwelch_harrison_array, threshold);
    carrier_smalley(index,:,:) = find_best_carriers(pwelch_smalley_array, threshold);
    carrier_camacho(index,:,:) = find_best_carriers(pwelch_camacho_array, threshold);
    carrier_conference(index,:,:) = find_best_carriers(pwelch_conference_array, threshold);
    carrier_chambers(index,:,:) = find_best_carriers(pwelch_chambers_array, threshold);


    figure(spot)
    hold on;
    % for carrier = 1:64
    % 
    foo = carrier_harrison(index,:,:);
    foo = squeeze(foo);
    bar(foo);
%         bar(carrier_harrison(index,:,:))%,'DisplayStyle','stairs','BinMethod','integers');
%         histogram(carrier_smalley(index,:,:),'DisplayStyle','stairs','BinMethod','integers');
%         histogram(carrier_camacho(index,:,:),'DisplayStyle','stairs','BinMethod','integers');
%         histogram(carrier_conference(index,:,:),'DisplayStyle','stairs','BinMethod','integers');
%         histogram(carrier_chambers(index,:,:),'DisplayStyle','stairs','BinMethod','integers');

    % end

    ylim([0,4000]);

    legend('Harrison','Smalley','Camacho','Conference','Chambers');
    hold off;
end

pr_harrison = carrier_harrison./4000;
pr_smalley = carrier_smalley./4000;
pr_camacho = carrier_camacho./4000;
pr_conference = carrier_conference./4000;
pr_chambers = carrier_chambers./4000;
