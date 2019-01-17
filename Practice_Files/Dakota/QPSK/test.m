clear;
load('linearSignalReducedCarriers.mat');
load('tx2_linear_signal');
load('linear_noisefloor.mat');
close all

num_loops = 1000;
tx2_linear_signal = sqrt(tx2_linear_signal);
harrison = tx2_linear_signal(:,36:65,65:98);
smalley = tx2_linear_signal(:,34:65,123:147);
camacho = tx2_linear_signal(:,34:65,154:179);
% harrison_noise = linear_noisefloor(:,36:65,65:98);
% smalley_noise = linear_noisefloor(:,34:65,123:147);
% camacho_noise = linear_noisefloor(:,34:65,154:179);

har_cap = zeros(1,num_loops);
smal_cap = zeros(1,num_loops);
cam_cap = zeros(1,num_loops);
har_best_x = zeros(1,num_loops);
har_best_y = zeros(1,num_loops);
smal_best_x = zeros(1,num_loops);
smal_best_y = zeros(1,num_loops);
cam_best_x = zeros(1,num_loops);
cam_best_y = zeros(1,num_loops);

% sigma2 = 1e-5;
snr = logspace(-3,7,num_loops);
% parfor sigma2 = logspace(-2,4,num_loops)
% for sigma2 = linspace(200,450,num_loops)
parfor index = 1:num_loops
    tic;
%     index = index + 1
    disp(index);
    sigma2 = snr(index);
    har = zeros(30,34);
    smal = zeros(32,25);
    cam = zeros(32,36);
    
    for row = 1:30
        for col = 1:34
            for carrier = 11:55
                har(row,col) = har(row,col) + 1 - probability_erasure(snr(index),...
                    harrison(carrier,row,col),.01);
            end
        end
    end
    
    for row = 1:32
        for col = 1:25
            for carrier = 11:55
                smal(row,col) = smal(row,col) + 1 - probability_erasure(snr(index)...
                    ,smalley(carrier,row,col),.01);
                cam(row,col) = cam(row,col) + 1 - probability_erasure(snr(index)...
                    ,camacho(carrier,row,col),.01);
                if col == 25
                    cam(row,col+1) = cam(row,col+1) + 1 - probability_erasure(snr(index)...
                    ,camacho(carrier,row,col+1),.01);
                end
            end
        end
    end
    
    
    [harrison_max,I] = max(har);
    [harrison_max,J] = max(harrison_max);
    har_cap(index) = harrison_max;
    har_best_x(index) = I(J);
    har_best_y(index) = J;
    
    [smalley_max,I] = max(smal);
    [smalley_max,J] = max(smalley_max);
    smal_cap(index) = smalley_max;
    smal_best_x(index) = I(J);
    smal_best_y(index) = J;
    
    [camacho_max,I] = max(cam);
    [camacho_max,J] = max(camacho_max);
    cam_cap(index) = camacho_max;
    cam_best_x(index) = I(J);
    cam_best_y(index) = J;
    % = [I(J) J];
    
    toc;
end
save('test1_workspace_new');
%%
har_best = [har_best_x' har_best_y'];
cam_best = [cam_best_x' cam_best_y'];
smal_best = [smal_best_x' smal_best_y'];
q_har_cap = zeros(1,num_loops);
q_smal_cap = zeros(1,num_loops);
q_cam_cap = zeros(1,num_loops);
parfor index = 1:num_loops
    sigma2 = snr(index);
    for carrier = 1:64
        q_har_cap(index) = q_har_cap(index) + 1 - erasure_probability(snr(index)...
        * harrison_noise(carrier,har_best(index,1),har_best(index,2)),...
        harrison(carrier,har_best(index,1),har_best(index,2)),.01);
    
        q_cam_cap(index) = q_cam_cap(index) + 1 - erasure_probability(snr(index)...
        * camacho_noise(carrier,cam_best(index,1),cam_best(index,2)),...
        camacho(carrier,cam_best(index,1),cam_best(index,2)),.01);
    
        q_smal_cap(index) = q_smal_cap(index) + 1 - erasure_probability(snr(index)...
        * smalley_noise(carrier,smal_best(index,1),smal_best(index,2)),...
        smalley(carrier,smal_best(index,1),smal_best(index,2)),.01);
    end
end
%     for carrier = 1:64
%     har_cap(index) = har_cap(index) + 1 - probability_erasure(sigma2,harrison(carrier,21,1),.01);
%     smal_cap(index) = smal_cap(index) + 1 - probability_erasure(sigma2,smalley(carrier,31,22),.01);
%     cam_cap(index) = cam_cap(index) + 1 - probability_erasure(sigma2,camacho(carrier,26,1),.01);
%     end

%%
load test1_workspace-long-running-over-weekend.mat;

figure()
hold on;
plot(har_cap);
plot(smal_cap);
plot(cam_cap);
hold off



figure()
hold on
plot(har_cap - smal_cap);
plot(har_cap - cam_cap);
hold off



figure()
hold on
plot(snr,har_cap);
plot(snr,smal_cap);
plot(snr,cam_cap);
ylabel('Bits per channel use')
xlabel('Power attenuation');
set(gca,'XScale','log');
hold off

figure()
hold on
plot(snr,har_cap - smal_cap);
plot(snr,har_cap - cam_cap);
ylabel('Secure bits per channel use')
xlabel('Power attenuation');
set(gca,'XScale','log');
hold off
% conference = linearSignalReducedCarriers(:,1:33,1:98);
% conf = squeeze(mean(conference));
% index = 1;
% for row = 1:33
%     for col = 1:98
%         X(index) = row;
%         Y(index) = col;
%         Z(index) = conf(row,col);
%         index = index + 1;
%     end
% end
% figure();
% scatter3(X,Y,Z);
% xlabel("row");
% ylabel("col");
% zlabel("G");
% % zlim([0,1])
% % [conf2,K] = mean(conf);
% harrison = linearSignalReducedCarriers(:,36:65,65:98);
% har = squeeze(mean(harrison));
% index = 1;
% for row = 1:num_loops
%     for col = 1:34
%         X(index) = row;
%         Y(index) = col;
%         Z(index) = har(row,col);
%         index = index + 1;
%     end
% end
% figure();
% scatter3(X,Y,Z);
% xlabel("row");
% ylabel("col");
% zlabel("G");
% % zlim([0,1])
% % [har2,J] = mean(har);
% smalley = linearSignalReducedCarriers(:,34:65,123:147);
% sma = squeeze(mean(smalley));
% index = 1;
% for row = 1:32
%     for col = 1:25
%         X(index) = row;
%         Y(index) = col;
%         Z(index) = sma(row,col);
%         index = index + 1;
%     end
% end
% figure();
% scatter3(X,Y,Z);
% xlabel("row");
% ylabel("col");
% zlabel("G");
% % zlim([0,1])
% % [sma2,K] = mean(sma);
% camacho = linearSignalReducedCarriers(:,34:65,154:179);
% cam = squeeze(mean(camacho));
% index = 1;
% for row = 1:32
%     for col = 1:26
%         X(index) = row;
%         Y(index) = col;
%         Z(index) = cam(row,col);
%         index = index + 1;
%     end
% end
% figure();
% scatter3(X,Y,Z);
% xlabel("row");
% ylabel("col");
% zlabel("G");
% % zlim([0,1])
% % [cam2,K] = mean(cam);
% chambers = linearSignalReducedCarriers(:,34:65,213:240);
% cham = squeeze(mean(chambers));
% index = 1;
% for row = 1:32
%     for col = 1:28
%         X(index) = row;
%         Y(index) = col;
%         Z(index) = cham(row,col);
%         index = index + 1;
%     end
% end
% figure();
% scatter3(X,Y,Z);
% xlabel("row");
% ylabel("col");
% zlabel("G");
% % zlim([0,1])
% % [cham2,K] = mean(cham);
