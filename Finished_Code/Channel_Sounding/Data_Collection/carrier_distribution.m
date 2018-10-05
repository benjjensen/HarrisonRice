close all;

load("StationaryData/pwelch_harrison_array.mat");
load("StationaryData/pwelch_smalley_array.mat");
load("StationaryData/pwelch_camacho_array.mat");
load("StationaryData/pwelch_conference_array.mat");
load("StationaryData/pwelch_chambers_array.mat");
spot = 0;

% % Uncomment this block to calculate the distribution of the number of
% % carriers that each room gets
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
carrier_harrison = zeros(71,64,1);
carrier_smalley = zeros(71,64,1);
carrier_camacho = zeros(71,64,1);
carrier_conference = zeros(71,64,1);
carrier_chambers = zeros(71,64,1);
index = 0;
for threshold = 25:.1:32
    index = index + 1;
    spot = spot + 1;
    
    carrier_harrison(index,:,:) = find_best_carriers(pwelch_harrison_array, threshold);
    carrier_smalley(index,:,:) = find_best_carriers(pwelch_smalley_array, threshold);
    carrier_camacho(index,:,:) = find_best_carriers(pwelch_camacho_array, threshold);
    carrier_conference(index,:,:) = find_best_carriers(pwelch_conference_array, threshold);
    carrier_chambers(index,:,:) = find_best_carriers(pwelch_chambers_array, threshold);

% % Uncomment this block to plot bar graphs of how many times of the 4000 
% % samples each carrier is good for all of the different dB thresholds
% % 
%     figure(spot)
%     hold on;
%     bar(carrier_harrison(index,:,1))%,'DisplayStyle','stairs','BinMethod','integers');
%     bar(carrier_smalley(index,:,1))%,'DisplayStyle','stairs','BinMethod','integers');
%     bar(carrier_camacho(index,:,1))%,'DisplayStyle','stairs','BinMethod','integers');
%     bar(carrier_conference(index,:,1))%,'DisplayStyle','stairs','BinMethod','integers');
%     bar(carrier_chambers(index,:,1))%,'DisplayStyle','stairs','BinMethod','integers');
% 
% 
% 
%     ylim([0,4000]);
% 
%     legend('Harrison','Smalley','Camacho','Conference','Chambers');
%     hold off;
end

pr_harrison = carrier_harrison./4000;
pr_smalley = carrier_smalley./4000;
pr_camacho = carrier_camacho./4000;
pr_conference = carrier_conference./4000;
pr_chambers = carrier_chambers./4000;
% 
% 
%     figure(75);
%     hold on;
%     bar(pr_harrison);
%     bar(pr_smalley);
%     bar(pr_camacho);
%     bar(pr_conference);
%     bar(pr_chambers);
%     legend('Harrison','Smalley','Camacho','Conference','Chambers');
%     hold off

% % Uncomment this block to plot the 3d plots of the probabilities of each
% % carrier being good with respect to db thresholds
% figure(1)
% hold on
% surf(pr_harrison);
% zlim([0,1])
% hold off
% figure(2)
% hold on
% surf(pr_smalley)
% zlim([0,1])
% hold off
% figure(3)
% hold on
% surf(pr_camacho);
% zlim([0,1])
% hold off
% figure(4)
% hold on
% surf(pr_conference);
% zlim([0,1])
% hold off
% figure(5)
% hold on
% surf(pr_chambers);
% zlim([0,1])
% hold off