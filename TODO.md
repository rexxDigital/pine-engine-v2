* Prepare new frame buffer
* Render each entity with custom shader, embedding entity index within the color
* On click, read the color with glReadPixels (should be fine if we use that *only* on each mouse click event)
* Translate that color to the entity index, select the corresponding entity