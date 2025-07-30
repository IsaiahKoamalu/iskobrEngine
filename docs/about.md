---
layout: default
title: About
---
<style>
  .button-container {
    display: flex;
    justify-content: center;
    margin-top: 20px;
  }
  .button {
    display: inline-block;
    padding: 0.5em 1em;
    background-color: #0066cc;
    color: #000;
    text-decoration: none;
    border-radius: 4px;
    font-family: sans-serif;
    transition: background-color 0.3s ease, transform 0.3s ease;
  }
  .button:hover {
    background-color: #0056b3;
    transform: scale(1.05);
    text-decoration: none;
  }
  .about-image {
    display: block;
    margin: 1.5em auto;
    width: 50%;
    height: auto;
    border: 10px solid #333;
    padding: 4px;
    border-radius: 6px;
  }
</style>

<div class="button-container">
  <a href="{{ '/' | relative_url }}" class="button">Main</a>
</div>

<img src="Images/aboutMeImage.jpg" alt="image" width="50%" height="50%"
  style="border: 10px solid #333; padding: 4px; border-radius: 6px;">

Hello!
-
My name is Isaiah Broderson, I am a computer science student currently studying at the University of Hawai'i Hilo. This is a personal project I have decided to undertake with the hopes of gaining a deeper understanding of data-oriented design and systems-level programming while also testing and expanding upon my knowledge of mathematics, physics, and logical problem solving. Although this project is still in its early stages I have found it has allowed me to really get a good feel for the importance of performance optimization and see first-hand the cause and effect of faulty memory management. I have been able to experience the root fundamentals of how data relates to and interacts with systems, which in turn has led to a strengthining in my understanding of software-hardware interaction.
