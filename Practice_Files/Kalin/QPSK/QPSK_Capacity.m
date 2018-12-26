clear;
close all;

load truncated_linear_signal.mat;

%% Capacity
noise = .01;

cpacity = (1/2) * log2(1 + (truncated_linear_signal / noise));

for y = 1:90
    for z = 1:345
        capacity(y,z) = sum(cpacity(:,y,z));
    end
end
save('capacity.mat','capacity');

harrison_max = max(max(capacity(36:65,65:98)));
[X Y] = find(capacity(36:65,65:98) == harrison_max);
max_X = X + 36 - 1;
max_Y = Y + 65 - 1;
%% Secrecy Capacity

% This section is currently incomplete. The code below is copied over from
% the previous capacity file and needs to be updated to match how secrecy
% capacity will be calculated for QPSK and our data now. 

% for x = 1:90
%     for y = 1:345
%         tx2_secrecy_capacity(x,y) = harrison_max_capacity - tx2_capacity(x,y);
%         tx2_adjusted_secrecy_capacity(x,y) = tx2_adjusted_harrison_max - tx2_adjusted_capacity(x,y);
%         if (tx2_secrecy_capacity(x,y) < 0)
%             tx2_secrecy_capacity(x,y) = 0;
%         end
%         if (tx2_adjusted_secrecy_capacity(x,y) < 0)
%             tx2_adjusted_secrecy_capacity(x,y) = 0;
%         end
%     end
% end
% save('tx2_secrecy_capacity.mat','tx2_secrecy_capacity');
% save('tx2_adjusted_secrecy_capacity.mat','tx2_adjusted_secrecy_capacity');
