Computational Aspects of Geometric Design hw3-----impelement subdivision(Loop and Catmull-Clark)
Hai Lin ___uni: hl2656
Purpose: implement subdivision surfaces of two types: both Loop and Catmull-Clark Subds for display in openGL.

How to run:
Type make to compile
Work both in Mac OS X and Columia CLIC 
subd_hl2656 + testfile
1-----flat normal
2-----average normal weighted by angle
>-----increase one level of subdivison
<-----decrease one level of subdivision


GL Display:

Loop subdivision-----GL_TRIANGLES
Catmull-Clark Subdivision----GL_POLYGON(0 level), Gl_QUADS(>=1 level)




