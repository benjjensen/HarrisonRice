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
    [mc_cam,~] = max(find_num_carriers(tx2camacho,threshold));
    [mc_chambers,~] = max(find_num_carriers(tx2chambers,threshold));
    [mc_conference,~] = max(find_num_carriers(tx2conference,threshold));
    [mc_harrison,~] = max(find_num_carriers(tx2harrison,threshold));
    [mc_smalley,~] = max(find_num_carriers(tx2smalley,threshold));
    
    mm_cam(i,1) = mc_cam;
    mm_cham(i,1) = mc_chambers;
    mm_conf(i,1) = mc_conference;
    mm_har(i,1) = mc_harrison;
    mm_smal(i,1) = mc_smalley;
    
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

% max_carrier_plot();