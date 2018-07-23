function [max_cam,max_cham,max_conf,max_harrison,max_smalley] = find_max_matched(camacho,chambers,conference,harrison,smalley)
%UNTITLED5 Summary of this function goes here
%   Detailed explanation goes here
cam = 0;
cham = 0;
conf = 0;
har = 0;
smal = 0;
for carrier = 1:64
   if harrison(1,carrier) ~= 0
       har = har + 1;
       if camacho(1,carrier) ~= 0
           cam = cam + 1;
       end
       if chambers(1,carrier) ~= 0
           cham = cham + 1;
       end
       if conference(1,carrier) ~= 0
           conf = conf + 1;
       end
       if smalley(1,carrier) ~= 0
           smal = smal + 1;
       end
   end
end
max_cam = cam;
max_cham = cham;
max_conf = conf;
max_harrison = har;
max_smalley = smal;
end

