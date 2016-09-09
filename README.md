# webrtcresearch
for research webrtc code


Fix Build Error:


I got this error in branch 51. I found the problem is md5sum must link to libatomic when use gcc 5.x version. So, i modify the file <dir of webrtc>/src/chromium/src/tools/android/md5sum/md5sum.gyp as follows:

diff --git a/tools/android/md5sum/md5sum.gyp b/tools/android/md5sum/md5sum.gyp
index 9099ba9..177883d 100644  
--- a/tools/android/md5sum/md5sum.gyp  
+++ b/tools/android/md5sum/md5sum.gyp  
@@ -76,6 +76,18 @@  
       'sources': [  
         'md5sum.cc',  
       ],  
+      'conditions': [  
+        [  
+            "OS=='android'",  
+            {  
+            "link_settings": {    
+                "libraries": [  
+                    "-latomic"
+                    ]  
+                }  
+            }  
+        ],  
+      ],  
     },  
   ],   
 }   

then cd to <dir of webrtc>/src and run:

python webrtc/build/gyp_webrtc.py
ninja -C out/Debug AppRTCDemo


