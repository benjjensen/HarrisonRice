close all;

load('camacho.mat');
load('chambers.mat');
load('conference.mat');
load('hall_pwelch.mat');
load('harrison.mat');
load('smalley.mat');
load('tx2camacho.mat');
load('tx2chambers.mat');
load('tx2conference.mat');
load('tx2hallway.mat');
load('tx2harrison.mat');
load('tx2smalley.mat');

mm_cam = zeros(351,1);
mm_cham = zeros(351,1);
mm_conf = zeros(351,1);
mm_har = zeros(351,1);
mm_smal = zeros(351,1);
i = 0;
for threshold = 0:.1:35
    i = i + 1;
    cam2 = find_num_carriers(tx2camacho,threshold);
    chambers2 = find_num_carriers(tx2chambers,threshold);
    conference2 = find_num_carriers(tx2conference,threshold);
    harrison2 = find_num_carriers(tx2harrison,threshold);
    smalley2 = find_num_carriers(tx2smalley,threshold);
    
%     camacho_best_carrier = find_best_carriers(cam1,camacho,3,threshold);
    camacho_best_carrier2 = find_best_carriers(cam2,tx2camacho,3,threshold);
%     chambers_best_carrier = find_best_carriers(chambers1,chambers,2,threshold);
    chambers_best_carrier2 = find_best_carriers(chambers2,tx2chambers,2,threshold);
%     conference_best_carrier = find_best_carriers(conference1,conference,1,threshold);
    conference_best_carrier2 = find_best_carriers(conference2,tx2conference,1,threshold);
%     harrison_best_carrier = find_best_carriers(harrison1,harrison,5,threshold);
    harrison_best_carrier2 = find_best_carriers(harrison2,tx2harrison,5,threshold);
%     smalley_best_carrier = find_best_carriers(smalley1,smalley,4,threshold);
    smalley_best_carrier2 = find_best_carriers(smalley2,tx2smalley,4,threshold);

    [mm_cam(i,1),mm_cham(i,1),mm_conf(i,1),...
        mm_har(i,1),mm_smal(i,1)] = find_max_matched(...
        camacho_best_carrier2,chambers_best_carrier2,...
        conference_best_carrier2,harrison_best_carrier2,...
        smalley_best_carrier2);
    
end

x = linspace(0,35,351);
figure(1)
hold on;
plot(x,mm_cam,'DisplayName','Camacho');
plot(x,mm_cham,'DisplayName','Chambers');
plot(x,mm_conf,'DisplayName','Conference Room');
plot(x,mm_har,'DisplayName','Harrison');
plot(x,mm_smal,'DisplayName','Smalley');
xlabel('DB Threshold');
ylabel('Number of Carriers');
% xlim(x);
legend;
hold off;
