% Plot the secrecy and transfer rate for different RM codes for the case where we choose the carriers based on the ones that give Dr. Harrison the biggest advantage.

% Assumptions:
%   -We round up when calculating the expected number of carriers the
%   evesdropper receives
%   -Don't worry at all about whether Harrison gets the carriers, only
%   worry about whether the evesdropper gets them.

close all;

load("../../Finished_Code/Channel_Sounding/Data_Collection/StationaryData/probabilities_harrison_advantage.mat", "probabilities_harrison_advantage");
load("../../Finished_Code/Channel_Sounding/Data_Collection/StationaryData/probabilities_smalley_carriers.mat", "probabilities_smalley_carriers");
load("../../Finished_Code/Channel_Sounding/Data_Collection/StationaryData/probabilities_harrison_carriers.mat", "probabilities_harrison_carriers");

numcarriers = 16;
m_min = 4;
m_max = 10;

code_info_array = [codeInfo];
code_info_array(m_max, m_max - m_min + 1, 71) = codeInfo;

expected_received_carriers = zeros(71,1);
% dB level == 24.9 + threshold / 10
for threshold = 1:71
    probs_indices = probabilities_harrison_advantage(:,threshold);
    probs_indices(:,2) = (1:64)';
    probs_indices = sortrows(probs_indices, 'descend');
    carriers = zeros(1,numcarriers);
    for i = 1:numcarriers
        carriers(1,i) = probs_indices(i, 2);
    end
    
    harrison_expected_carriers = 0;
    for carrier = carriers(1,:)
       harrison_expected_carriers = harrison_expected_carriers + probabilities_harrison_carriers(carrier, threshold); 
    end
    
    probs_eve = probabilities_smalley_carriers(:,threshold);
    expected_received_carriers(threshold) = 0;
    for carrier = carriers(1,:)
        expected_received_carriers(threshold) = expected_received_carriers(threshold) + probs_eve(carrier);
    end
    received_bits = ceil(expected_received_carriers(threshold));
    
    equivocation_proportion = ones(10, 10) * -1;
    for m = m_min:m_max
        for r = 1:m
            weights = RMWeightHier(r, m, false);
            
            code_info = codeInfo;
            
            code_info.m = m;
            code_info.u = r;
            code_info.k = weights(1,1);
            code_info.n = 2^m;
            code_info.rate = code_info.k / code_info.n;
            code_info.dBLevel = threshold / 10 + 24.9;
            code_info.mu = received_bits;
            code_info.H = weights(1, received_bits + 1);
            code_info.percentLeaked = 100 * (code_info.k - code_info.H) / code_info.k;
            code_info.carrierRate = code_info.rate * (harrison_expected_carriers / numcarriers);
            
            code_info_array(r, m - m_min + 1, threshold) = code_info;
        end
    end
end

close all;
figure(1);
hold on;

nontrivial_code_count = 0;
for j = m_min : m_max
    nontrivial_code_count = nontrivial_code_count + j;
end
nontrivial_code_count = nontrivial_code_count - m_max + m_min - 1;

colors = jet(71);

for threshold = 1:71
    rates = zeros(nontrivial_code_count, 1);
    secrecies = zeros(nontrivial_code_count, 1);
    code_names = categorical(nontrivial_code_count, 1);
    
    counter = 1;
    for m = m_min:m_max
        for u = 1:m-1
            code_info = code_info_array(u, m - m_min + 1, threshold);
            rates(counter, 1) = code_info.carrierRate;
            secrecies(counter, 1) = 100 * (1 - code_info.percentLeaked);
            name = sprintf("RM(%d,%d) %.1f dB", u, m, threshold/10 + 24.9);
            code_names(counter, 1) = categorical(cellstr(name));
            
            counter = counter + 1;
        end
    end
    
    scatter3(rates, secrecies, code_names, 10, colors(threshold, :), 'o');
end

title('Code Efficiency');
xlabel('Throughput Rate');
ylabel('Equivocation (%)');
zlabel('Reed-Muller Code (r,m)');
ylim([0 100]);

hold off;

