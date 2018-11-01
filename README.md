# HW1-simple-pstree

## kernel module  
  編譯  
    $make  
  install module  
    $make ins  
  remove module  
    $make rm  
## user program  
  編譯  
    $make  

## pre-commit  
  astyle --style=kr --indent=spaces=4 --suffix=none simple_pstree.c  

## git command  
  git add .  
  git commit -m "Comments here"  
  git push -u origin master  
  
## 作業完成:  
* 基本要求  
* bonus  
    a.透過thread_group去尋找thread  
    b.當中可以完成thread的thread的thread...的children或sibling尋找  
    c.-s sibling的尋找方式為先找parent，再從parent的children還有parent的thread的children去找  
    d.-c children的尋找方式為先找task的children，再找task的thread(多層thread)的children  
    e.-p parent的尋找不需要處理到thread的部分  
    
## 其他注意事項
-s 時不會顯示自己，符合助教範例
