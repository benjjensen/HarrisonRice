clear; close all;

tic;

load linear_signal.mat;
for y = 1:90
    for z = 1:345
        linear_signal(:,y,z) = fftshift(linear_signal(:,y,z));
    end
end

temp(:,:) = linear_signal(1,:,:);

gc_signal = linear_signal(11:55,:,:);

g = sqrt(gc_signal);
for y = 1:90
    for z = 1:345
        g_norm(:,y,z) = g(:,y,z) .* max(g(:,y,z));
        g_squared(:,y,z) = g_norm(:,y,z) .^ 2;
    end
end

% SNRdB = linspace(-20, 20, 1000);
% SNR = 10 .^ (SNRdB / 10);
numSNRpoints = 300;
SNR_lin = logspace(2, 5, numSNRpoints);
SNR = 10*log10(SNR_lin);

tau = 10^(5/10); % This should be between -10 and 10 dB, the value chosen here is 5 dB
parfor sweep = 1:numSNRpoints
    % Bobs room is found at g_squared(:,36:65,65:98)
    disp(string(sweep) + ' / ' + string(numSNRpoints) + ' g*snr array');
    for y = 1:90
        for z = 1:345
            for x = 1:45
                if isnan(g_squared(x,y,z))
                    g_times_snr(sweep, x, y, z) = nan;
                else
                    g_times_snr(sweep, x, y, z) = g_squared(x,y,z) * SNR_lin(sweep);
                end
            end
        end
    end
end

result_eve1 = g_times_snr(:,:,1:65,1:61);
result_eve2 = g_times_snr(:,:,1:65,123:148);
result_eve3 = g_times_snr(:,:,1:65,154:179);
result_eve4 = g_times_snr(:,:,1:65,213:240);
result_bob = g_times_snr(:,:,1:65,65:98);
result_hall = g_times_snr(:,:,66:90,:);

parfor sweep = 1:numSNRpoints
    disp(string(sweep) + ' / ' + string(numSNRpoints) + ' separate into rooms and see if above threshold part 1/4');
    for x = 1:45
        for y = 1:65
            for z = 1:61
                if isnan(result_eve1(sweep,x,y,z))
                    gc_eve1(sweep,x,y,z) = nan;
                elseif result_eve1(sweep,x,y,z) >= tau
                    gc_eve1(sweep,x,y,z) = 1;
                else
                    gc_eve1(sweep,x,y,z) = 0;
                end
            end
        end
    end
end

parfor sweep = 1:numSNRpoints
    disp(string(sweep) + ' / ' + string(numSNRpoints) + ' separate into rooms and see if above threshold part 2/4');
    for x = 1:45
        for y = 1:65
            for z = 1:26
                if isnan(result_eve2(sweep,x,y,z))
                    gc_eve2(sweep,x,y,z) = nan;
                elseif result_eve2(sweep,x,y,z) >= tau
                    gc_eve2(sweep,x,y,z) = 1;
                else
                    gc_eve2(sweep,x,y,z) = 0;
                end
                if isnan(result_eve3(sweep,x,y,z))
                    gc_eve3(sweep,x,y,z) = nan;
                elseif result_eve3(sweep,x,y,z) >= tau
                    gc_eve3(sweep,x,y,z) = 1;
                else
                    gc_eve3(sweep,x,y,z) = 0;
                end
            end
        end
    end
end

parfor sweep = 1:numSNRpoints
    disp(string(sweep) + ' / ' + string(numSNRpoints) + ' separate into rooms and see if above threshold part 3/4');
    for x = 1:45
        for y = 1:65
            for z = 1:28
                if isnan(result_eve4(sweep,x,y,z))
                    gc_eve4(sweep,x,y,z) = nan;
                elseif result_eve4(sweep,x,y,z) >= tau
                    gc_eve4(sweep,x,y,z) = 1;
                else
                    gc_eve4(sweep,x,y,z) = 0;
                end
            end
        end
    end
end

parfor sweep = 1:numSNRpoints
    disp(string(sweep) + ' / ' + string(numSNRpoints) + ' separate into rooms and see if above threshold part 4/4');
    for x = 1:45
        for y = 1:65
            for z = 1:34
                if isnan(result_bob(sweep,x,y,z))
                    gc_bob(sweep,x,y,z) = nan;
                elseif result_bob(sweep,x,y,z) >= tau
                    gc_bob(sweep,x,y,z) = 1;
                else
                    gc_bob(sweep,x,y,z) = 0;
                end
            end
        end
    end
end

parfor sweep = 1:numSNRpoints
    disp(string(sweep) + ' / ' + string(numSNRpoints) + ' separate into rooms and see if above threshold part 5/4');
    for x = 1:45
        for y = 1:25
            for z = 1:345
                if isnan(result_hall(sweep,x,y,z))
                    gc_hall(sweep,x,y,z) = nan;
                elseif result_hall(sweep,x,y,z) >= tau
                    gc_hall(sweep,x,y,z) = 1;
                else
                    gc_hall(sweep,x,y,z) = 0;
                end
            end
        end
    end
end

save gc_eve1.mat  gc_eve1;
save gc_eve2.mat  gc_eve2;
save gc_eve3.mat  gc_eve3;
save gc_eve4.mat  gc_eve4;
save gc_bob.mat  gc_bob;
save gc_hall.mat  gc_hall;
save SNR.mat SNR;

toc;