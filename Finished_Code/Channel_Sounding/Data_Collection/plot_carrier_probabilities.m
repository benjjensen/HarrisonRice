close all;

load("StationaryData/probabilities_camacho_carriers.mat", "probabilities_camacho_carriers");
load("StationaryData/probabilities_chambers_carriers.mat", "probabilities_chambers_carriers");
load("StationaryData/probabilities_conference_carriers.mat", "probabilities_conference_carriers");
load("StationaryData/probabilities_harrison_carriers.mat", "probabilities_harrison_carriers");
load("StationaryData/probabilities_smalley_carriers.mat", "probabilities_smalley_carriers");

plot_probabilities(probabilities_camacho_carriers, "Camacho Carrier Probabilities");
plot_probabilities(probabilities_chambers_carriers, "Chambers Carrier Probabilities");
plot_probabilities(probabilities_conference_carriers, "Conference Room Carrier Probabilities");
plot_probabilities(probabilities_harrison_carriers, "Harrison Carrier Probabilities");
plot_probabilities(probabilities_smalley_carriers, "Smalley Carrier Probabilities");

probabilities_harrison_advantage = probabilities_harrison_carriers - probabilities_smalley_carriers;
plot_probabilities(probabilities_harrison_carriers - probabilities_smalley_carriers, "Harrison Advantage");

function plot_probabilities(probability_array, plottitle)
    figure();
    surf(probability_array);
    title(plottitle);
    xlabel("Power Threshold (dB)");
    xlim([1,71])
    xticks([1 11 21 31 41 51 61 71])
    xticklabels({'25','26','27','28','29','30','31','32'})
    ylabel("Carrier Index");
    ylim([1,64]);
    zlabel("Probability");
    zlim([0,1]);
end