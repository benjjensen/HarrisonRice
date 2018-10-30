close all
clear

for bl = 1:2:19
    eval(sprintf('load("mu_%d_2");',bl));
    eval(sprintf('norm_mu_%d = normalizedMus(mu_%d_2);',bl,bl));
    eval(sprintf('norm_sum_mu_%d = normalizedMuSum(mu_%d_2);',bl,bl));

end

[num_rows,~] = size(mu_1_2);

for row = 1:num_rows
    figure(row);
    hold on;
%     for bl = 10:20
%         eval(sprintf('plot(norm_mu_%d,mu_normal_%d(row,:));',bl,bl));
%     end
    for bl = 1:2:19
        eval(sprintf('plot(norm_mu_%d,norm_sum_mu_%d(row,:));',bl,bl));
    end
    xlabel('Revealed Bits (%)');
    ylabel('Probability (%)');
    xlim([0,1]);
    ylim([0,1]);
    legend('M = 1', 'M = 3', 'M = 5', 'M = 7', 'M = 9', 'M = 11', ...
    'M = 13', 'M = 15', 'M = 17', 'M = 19');
    hold off
end