load("StationaryData/pwelch_camacho_array.mat");
load("StationaryData/pwelch_chambers_array.mat");
load("StationaryData/pwelch_conference_array.mat");
load("StationaryData/pwelch_harrison_array.mat");
load("StationaryData/pwelch_smalley_array.mat");

probabilities_camacho_carriers = calc_carrier_probabilities(pwelch_camacho_array);
probabilities_chambers_carriers = calc_carrier_probabilities(pwelch_chambers_array);
probabilities_conference_carriers = calc_carrier_probabilities(pwelch_conference_array);
probabilities_harrison_carriers = calc_carrier_probabilities(pwelch_harrison_array);
probabilities_smalley_carriers = calc_carrier_probabilities(pwelch_smalley_array);

save("StationaryData/probabilities_camacho_carriers.mat", "probabilities_camacho_carriers");
save("StationaryData/probabilities_chambers_carriers.mat", "probabilities_chambers_carriers");
save("StationaryData/probabilities_conference_carriers.mat", "probabilities_conference_carriers");
save("StationaryData/probabilities_harrison_carriers.mat", "probabilities_harrison_carriers");
save("StationaryData/probabilities_smalley_carriers.mat", "probabilities_smalley_carriers");

function [carrier_probabilities] = calc_carrier_probabilities(pwelch_data)
    [~,num_runs] = size(pwelch_data);
    carrier_probabilities = zeros(64,71);
    threshold_count = 1;
    for threshold = 25:.1:32
        carrier_good_count = zeros(64,1);
        for run = (1:num_runs)
            run_data = pwelch_data(:,run);
            for carrier_index = (1:64)
                signal_level = run_data(carrier_index * 2 - 1);
                noise_floor = run_data(carrier_index * 2);
                if signal_level - noise_floor >= threshold
                   carrier_good_count(carrier_index,1) = carrier_good_count(carrier_index,1) + 1; 
                end
            end
        end

        carrier_good_probability = carrier_good_count / num_runs;
        carrier_probabilities(:,threshold_count) = carrier_good_probability;
        threshold_count = threshold_count + 1;
    end
end