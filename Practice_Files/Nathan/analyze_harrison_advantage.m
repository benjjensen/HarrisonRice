addpath("../../Finished_Code/Channel_Sounding/Data_Collection");

load("StationaryData/probabilities_harrison_advantage.mat", "probabilities_harrison_advantage");

numcarriers = 16;

% dB level == 24.9 + threshold / 10
for threshold = 1:71
    probs_indices = probabilities_harrison_advantage(:,threshold);
    probs_indices(:,2) = (1:64)';
    probs_indices = sortrows(probs_indices, 'descend');
    carriers = zeros(1,numcarriers);
    for i = 1:numcarriers
        carriers(i) = probs_indices(i, 2);
    end
    
    
end

